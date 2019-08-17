//--------------------------------------------------------------
// wiring with UNO or Mega2560:
//--------------POWER Pins--------------------------------------
//   5V  connects to DC 5V
//   GND connects to Ground
//   3V3 do not need to connect(NC)
//--------------LCD Display Pins--------------------------------
//   LCD_RD   connects to Analog pin A0
//   LCD_WR   connects to Analog pin A1
//   LCD_RS   connects to Analog pin A2
//   LCD_CS   connects to Analog pin A3
//   LCD_RST  connects to Analog pin A4
//   LCD_D0   connects to digital pin 8
//   LCD_D1   connects to digital pin 9
//   LCD_D2   connects to digital pin 2
//   LCD_D3   connects to digital pin 3
//   LCD_D4   connects to digital pin 4
//   LCD_D5   connects to digital pin 5
//   LCD_D6   connects to digital pin 6
//   LCD_D7   connects to digital pin 7
//--------------SD-card fuction Pins ---------------------------
// This Connection Only for UNO, Do not support Mega2560
// because they use different Hardware-SPI Pins
// SD_SS    connects to digital pin 10
// SD_DI    connects to digital pin 11
// SD_DO    connects to digital pin 12
// SD_SCK   connects to digital pin 13
//--------------------------------------------------------------

// typedef struct {
//  byte stx; // allways STX
//  byte len; // reserved
//-------------------------------
// payload
//-------------------------------
//  word w_0; // -- setpoint teplota
//  word w_1; // -- skutocna teplota
//  word w_2; // -- aktualny osvit
//  word w_3; // -- reserved
//  word w_4; // -- reserved
//  word w_5; // -- reserved
//  word w_6; // -- tx setpoint teplota
//  word w_7; // -- stav vystupov
//  word w_8; // -- feedbacky
//  word w_9; // -- prikazy
//-------------------------------
// end of payload
//-------------------------------
//  byte crc; // reserved
//  byte etx; // allways ETX
//} TELEGRAM;

// Knižnice
#include <telegram.h>
#include <motolib.h>
#include <house.h>
#include <television.h>

// funkcia pre nacitanie stavu vystupov
int digitalReadOutputPin(uint8_t pin);
// zaluzie
Louver louver;
// garaz
Garage garage;
// svetlo
Light light;
// teplota
Temp temp;
//televizor
Television television;
//server
Server server;

//--------------------------------------------------------------
// nacitaj vstupy z Arduina
//--------------------------------------------------------------
void readInputs()
{
    //************************************************************
    // načtení hodnoty z analogového pinu
    // Světelný senzor TEMT6000
    // vytvoření proměnných pro výsledky měření
    int osvit = light.doLightReadExposure();
    television.setExposition(osvit);

    // přepočet analogové hodnoty z celého rozsahu
    //  0-1023 na procentuální rozsah 0-100
    int prepocet = map(osvit, 0, 1023, 0, 100);
    // zapamataj vypocitany osvit
    server.telegram.msg.w[Server::ATB] = (word)prepocet;
    //************************************************************
    // TEPLOMER
    temp.doRead();
    float teplota = temp.getTemp();
    television.setSetTemp((int)teplota);
    // zapamataj aktualnu teplotu
    server.telegram.msg.w[Server::ATM] = teplota;
    //************************************************************
    // nacitaj stav binarnych vstupov
    // nastavenie koncovych poloh garaze otvorena poloha
    if (digitalRead(OPEN_LIMIT_PIN) == HIGH)
    { // kontakt otvorene true
        garage.setOpened(true);
        television.setOpened(true);
        BITMASK_CLEAR(server.telegram.msg.w[Server::FDB], Server::FBK_GAR_ON);
    }
    else
    { // kontakt otvorene false
        garage.setOpened(false);
        television.setOpened(false);
        BITMASK_SET(server.telegram.msg.w[Server::FDB], Server::FBK_GAR_ON);
    }
    // nastavenie koncovych poloh garaze zatvorena poloha
    if (digitalRead(CLOSE_LIMIT_PIN) == HIGH)
    { // kontakt zatvorene true
        garage.setClosed(true);
        television.setClosed(true);
        BITMASK_CLEAR(server.telegram.msg.w[Server::FDB], Server::FBK_GAR_OFF);
    }
    else
    { // kontakt zatvorene false
        garage.setClosed(false);
        television.setClosed(false);
        BITMASK_SET(server.telegram.msg.w[Server::FDB], Server::FBK_GAR_OFF);
    }
}
//--------------------------------------------------------------
// aktualizovat vystupy Arduina
//--------------------------------------------------------------
void writeOutputs()
{
    // nastavenie vystupov , relatok
    //************************************************************
    bool onMoveToOpen = BITMASK_CHECK_ALL(server.telegram.msg.w[Server::STA], Server::STA_GAR_ON);
    bool onMoveToClose = BITMASK_CHECK_ALL(server.telegram.msg.w[Server::STA], Server::STA_GAR_OFF);
    // ak je poziadavka zatvorit
    if (onMoveToClose)
    {
        // pohni garaz na zatvorenie
        garage.doClose();
    }
    // ak je poziadavka zatvorit
    if (onMoveToOpen)
    {
        // pohni garaz na zatvorenie
        garage.doOpen();
    }

    //************************************************************
    // zapnutie svetiel
    // 12V zapnutie LED svetiel
    // ak sa ma svetlo zapnut
    if (BITMASK_CHECK_ALL(server.telegram.msg.w[Server::STA], Server::STA_LIT_ON))
    {
        light.doLightTurnOn();
        television.setLight(true);
    }
    // ak sa ma svetlo vypnut
    if (BITMASK_CHECK_ALL(server.telegram.msg.w[Server::STA], Server::STA_LIT_OFF))
    {
        light.doLightTurnOff();
        television.setLight(false);
    }
    // ak sa ma svetlo zapnut automaticky
    if (BITMASK_CHECK_ALL(server.telegram.msg.w[Server::STA], Server::STA_LIT_AUT))
    {
        // vyhodnotit osvit
        int i_val = (int)server.telegram.msg.w[Server::ATB];
        if (i_val < ZAPNUTIE_SVETIEL_HRANICA)
        {
            light.doLightTurnOn();
            television.setLight(true);
        }
        else
        {
            light.doLightTurnOff();
            television.setLight(false);
        }
        // vyhodnotit zatvorenie zaclon od osvitu
        if (i_val < ZATVORENIE_ZACLON_HRANICA)
        {
            louver.doLouverClose();
        }
        else
        {
            louver.doLouverOpen();
        }
    }
    // odlozit aktualny stav relatok do feedbacku
    if (digitalReadOutputPin(LIGHT_RELAY_PIN) == HIGH)
    {
        BITMASK_CLEAR(server.telegram.msg.w[Server::FDB], Server::FBK_RELE_1);
    }
    else
    {
        BITMASK_SET(server.telegram.msg.w[Server::FDB], Server::FBK_RELE_1);
    }
}
//--------------------------------------------------------------
// vyhodnot prikazy a vykonaj vypocty
//--------------------------------------------------------------
void urobitPrepocty()
{
    // ak sa nieco ma zmenit so svetlom
    // vyhodnotit prikazy zo servera
    //*******************************************
    // vyhodnotenie svetla
    word CMD_MASK = Server::CMD_LIT_ON | Server::CMD_LIT_OFF | Server::CMD_LIT_AUT;
    word STA_MASK = Server::STA_LIT_ON | Server::STA_LIT_OFF | Server::STA_LIT_AUT;
    // ak je niektory priznak z povelu pre svetlo aktivny

    // ak sa ma svetlo zapnut
    if (BITMASK_CHECK_ANY(server.telegram.msg.w[Server::CMD], Server::CMD_LIT_ON))
    {
        BITMASK_CLEAR(server.telegram.msg.w[Server::STA], STA_MASK);
        BITMASK_SET(server.telegram.msg.w[Server::STA], Server::STA_LIT_ON);
    }
    // ak sa ma svetlo vypnut
    if (BITMASK_CHECK_ANY(server.telegram.msg.w[Server::CMD], Server::CMD_LIT_OFF))
    {
        BITMASK_CLEAR(server.telegram.msg.w[Server::STA], STA_MASK);
        BITMASK_SET(server.telegram.msg.w[Server::STA], Server::STA_LIT_OFF);
    }
    // ak sa ma svetlo zapnut automaticky
    if (BITMASK_CHECK_ANY(server.telegram.msg.w[Server::CMD], Server::CMD_LIT_AUT))
    {
        BITMASK_CLEAR(server.telegram.msg.w[Server::STA], STA_MASK);
        BITMASK_SET(server.telegram.msg.w[Server::STA], Server::STA_LIT_AUT);
    }
    // koniec poziadaviek pre svetlo
    BITMASK_CLEAR(server.telegram.msg.w[Server::CMD], CMD_MASK);
    //*******************************************
    // vyhodnotenie pohybu garaze
    CMD_MASK = Server::CMD_GAR_ON | Server::CMD_GAR_OFF | Server::CMD_GAR_STOP;
    STA_MASK = Server::STA_GAR_ON | Server::STA_GAR_OFF | Server::STA_GAR_STOP;
    // zisti ktorym smerom sa garaz hybe
    bool statusOld = BITMASK_CHECK_ALL(server.telegram.msg.w[Server::STA], STA_MASK);
    // ak chcem aby sa garaz otvorila
    if (BITMASK_CHECK_ANY(server.telegram.msg.w[Server::CMD], Server::CMD_GAR_ON))
    {
        // nech sa zacne hybat k otvoreniu
        BITMASK_CLEAR(server.telegram.msg.w[Server::STA], STA_MASK);
        BITMASK_SET(server.telegram.msg.w[Server::STA], Server::STA_GAR_ON);
    }
    // ak chcem aby sa garaz zatvorila
    if (BITMASK_CHECK_ANY(server.telegram.msg.w[Server::CMD], Server::CMD_GAR_OFF))
    {
        // nech sa zacne hybat k zatvoreniu
        BITMASK_CLEAR(server.telegram.msg.w[Server::STA], STA_MASK);
        BITMASK_SET(server.telegram.msg.w[Server::STA], Server::STA_GAR_OFF);
    }
    // ak chcem aby sa garaz zatvorila
    if (BITMASK_CHECK_ANY(server.telegram.msg.w[Server::CMD], Server::CMD_GAR_STOP))
    {
        // nech sa zastavi
        BITMASK_CLEAR(server.telegram.msg.w[Server::STA], STA_MASK);
        BITMASK_SET(server.telegram.msg.w[Server::STA], Server::STA_GAR_STOP);
    }
    // koniec poziadaviek pre garaz
    BITMASK_CLEAR(server.telegram.msg.w[Server::CMD], CMD_MASK);
    //*******************************************
}

//--------------------------------------------------------------
// nacitaj aktualny stav vystupu
//--------------------------------------------------------------
int digitalReadOutputPin(uint8_t pin)
{
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
    if (port == NOT_A_PIN)
        return LOW;

    return (*portOutputRegister(port) & bit) ? HIGH : LOW;
}

//--------------------------------------------------------------
// nastavenie Arduina
//--------------------------------------------------------------
void setup()
{
    //ovladanie svetla
    light.setup();
    // nastavenie zaluzii
    louver.setup(PWM_B_PIN, IN1_B_PIN, IN2_B_PIN);
    louver.MaxSpeed = ZACLONY_SPEED;
    louver.DelayTransfer = ZACLONY_DELAY_TO_TRANSFER;

    // nastavenie garaze
    garage.setup(PWM_A_PIN, IN1_A_PIN, IN2_A_PIN);
    garage.MaxSpeedToOpen = GARAGE_SPEED_OPEN;
    garage.MaxSpeedToClose = GARAGE_SPEED_CLOSE;

    //************************************************************
    // nastavenie vstupov
    pinMode(OPEN_LIMIT_PIN, INPUT);  // kontakt otvorena garaz
    pinMode(CLOSE_LIMIT_PIN, INPUT); // kontakt zatvorena garaze
    //************************************************************
    // zahájení komunikace po sériové lince
    // rychlostí 115200 baud
    Serial.begin(115200);
    //************************************************************
    // nastavi displej
    television.setup();
}
//--------------------------------------------------------------
// hlavna slucka programu
//--------------------------------------------------------------
//nastavenie opakovacej slocky, cislo je v ms
const int LOOP_DELAY = 100;

void loop()
{
    //************************************************************
    // zapametaj predosly stav periferii
    TELEGRAM last_msg;
    memcpy(&last_msg, &server.telegram.msg, sizeof(server.telegram.msg));
    //************************************************************
    // nacitaj data zo serveru
    server.doReadMessage();
    //************************************************************
    // nacitaj vstupy z Arduina
    readInputs();
    //************************************************************
    // vyhodnot prikazy a vykonaj vypocty
    urobitPrepocty();
    //************************************************************
    // zobrazit data na displej
    television.drawScreen();
    //************************************************************
    // aktualizovat vystupy Arduina
    writeOutputs();
    //************************************************************
    // odoslat data do serveru
    server.doWriteMessage();
    //************************************************************
    server.telegram.msg.w[Server::CMD] = 0; // ziadne prikazy z nadradeneho servera
    // pauza pred nasledujucim cyklom
    delay(LOOP_DELAY);
}
