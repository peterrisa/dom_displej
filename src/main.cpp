//--------------------------------------------------------------
// wiring with Mega2560, display
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


// Knižnice
#include <telegram.h>
#include <motolib.h>
#include <house.h>
#include <television.h>
#include <server.h>

// Funkcia pre nacitanie stavu vystupov
int digitalReadOutputPin(uint8_t pin);
// žalúzie
Louver *louver;
// garaz
Garage *garage;
// svetlo
Light *light;
// teplota
Temp *temp;
// televizor
Television *television;
// server
Server *server;

//--------------------------------------------------------------
// nacitaj vstupy z Arduina
//--------------------------------------------------------------
void readInputs()
{
    //************************************************************
    // načítanie hodnoty z analogového pinu
    // svetelný senzor TEMT6000
    // vytvorenie premenných pre výsledok merania
    int osvit = light->doLightReadExposure();
    // prepočet analogovej hodnoty z celého rozsahu
    //  0-1023 na percentuálny rozsah 0-100
    int prepocet = map(osvit, 0, 1023, 0, 100);
    television->setExposition(prepocet);
    // zapamätaj vypočítany osvit
    server->setWord(Server::ATB,prepocet);
    //************************************************************
    // TEPLOMER
    temp->doRead();
    float teplota = temp->getTemp();
    television->setCurTemp((int)teplota);
    // zapamataj aktualnu teplotu
    server->setWord(Server::ATM,teplota);
    //************************************************************
    // načítaj stav binarnych vstupov
    // nastavenie koncových poloh garaze otvorena poloha
    if (digitalRead(OPEN_LIMIT_PIN) == LOW)
    { // kontakt otvorene = true
        garage->setOpened(true);
        television->setOpened(true);
        server->setBit(Server::FDB,Server::FBK_GAR_ON,true);
        //zastav povel na zapnutie otvarania
        server->setBit(Server::STA,Server::STA_GAR_ON,false);
    }
    else
    { // kontakt otvorene = false
        garage->setOpened(false);
        television->setOpened(false);
        server->setBit(Server::FDB,Server::FBK_GAR_ON,false);
    }
    // nastavenie koncovych polôh garáže zatvorena poloha
    if (digitalRead(CLOSE_LIMIT_PIN) == LOW)procentuální
    { // kontakt zatvorene = true
        garage->setClosed(true);
        television->setClosed(true);
        server->setBit(Server::FDB,Server::FBK_GAR_OFF,true);
        //zastav povel na zapnutie zatvarania
        server->setBit(Server::STA,Server::STA_GAR_OFF,false);
    }
    else
    { // kontakt zatvorene = false
        garage->setClosed(false);
        television->setClosed(false);
        server->setBit(Server::FDB,Server::FBK_GAR_OFF,false);
    }
}
//--------------------------------------------------------------
// aktualizovať výstupy Arduina
//--------------------------------------------------------------
void writeOutputs()
{
    // ak je požiadavka zatvoriť garáž
    if (server->getBit(Server::STA, Server::STA_GAR_OFF))
    {
        // pohni garáž na zatvorenie
        garage->doClose();
    }
    // ak je požiadavka otvoriť garáž
    if (server->getBit(Server::STA, Server::STA_GAR_ON))
    {
        // pohni garáž na otvorenie
        garage->doOpen();
    }
    bool GarageMotorRuning =  garage->isClosing() | garage->isOpening();
    server->setBit(Server::FDB,Server::FBK_GAR_STOP,GarageMotorRuning);

    // ak sa ma svetlo zapnúť automaticky
    if (server->getBit(Server::STA, Server::STA_LIT_AUT) || light->isAutomat())
    {
        light->doLightStartAutomat();
        // vyhodnotiť osvit
        int i_val = (int)server->getWord(Server::ATB);
        if (i_val < ZAPNUTIE_SVETIEL_HRANICA)
        {
            light->doLightTurnOn();
            television->setLight(true);
        }
        else
        {
            light->doLightTurnOff();
            television->setLight(false);
        }
        // vyhodnotit zatvorenie záclon od osvitu
        if (i_val < ZATVORENIE_ZACLON_HRANICA)
        {
            louver->doLouverClose();
        }
        else
        {
            louver->doLouverOpen();
        }
    }

    // ak sa ma svetlo zapnúť
    if (server->getBit(Server::STA, Server::STA_LIT_ON))
    {
        light->doLightTurnOn();
        light->doLightStopAutomat();
        television->setLight(true);
    }
    // ak sa ma svetlo vypnúť
    if (server->getBit(Server::STA, Server::STA_LIT_OFF))
    {
        light->doLightTurnOff();
        light->doLightStopAutomat();
        television->setLight(false);
    }
    
 // odlozit aktualny stav relátok do feedbacku
    server->setBit(Server::FDB,Server::FBK_LIT_ON,light->isLightOn());
    server->setBit(Server::FDB,Server::FBK_LIT_OFF,!light->isLightOn());
    server->setBit(Server::FDB,Server::FBK_LIT_AUT,light->isAutomat());
    server->setBit(Server::FDB,Server::FBK_RELE_1,light->isLightOn());
}

//--------------------------------------------------------------
// načítaj aktualny stav výstupu
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
    // žaluzie
    louver = new Louver;
    // garáž
    garage = new Garage;
    // svetlo
    light = new Light;
    // teplota
    temp = new Temp;
    //televízor "SmartTV"
    television = new Television;
    //server
    server = new Server;

    //************************************************************
    // nastaviť displej
    television->setup();
    //ovládanie svetla
    light->setup();
    // nastavenie zaluzii
    louver->setup(PWM_B_PIN, IN1_B_PIN, IN2_B_PIN);
    // nastavenie rýchlosti žalúzií
    louver->MaxSpeed = ZACLONY_SPEED;
    louver->DelayTransfer = ZACLONY_DELAY_TO_TRANSFER;

    // nastavenie garaze
    garage->setup(PWM_A_PIN, IN1_A_PIN, IN2_A_PIN);
    //nastavenie rýchlosti otvárania garáže (3/4)
    garage->MaxSpeedToOpen = GARAGE_SPEED_OPEN;
    //nastavenie rýclosti zatvárania garáže (maximum)
    garage->MaxSpeedToClose = GARAGE_SPEED_CLOSE;

    //************************************************************
    // nastavenie výstupov
    pinMode(OPEN_LIMIT_PIN, INPUT);  // kontakt otvorena garáže
    pinMode(CLOSE_LIMIT_PIN, INPUT); // kontakt zatvorena garáže
    //************************************************************
    // začatie komunikáce po sériovej linke
    // rychlosť 115200 baud
    Serial.begin(115200);
}
//--------------------------------------------------------------
// hlavná slučka programu
//--------------------------------------------------------------
//nastavenie opakovacej slučky, čislo je v ms
const int LOOP_DELAY = 100;

void loop()
{
    //čakaj na ďalsie povely zo servera
    server->setWord(Server::CMD, Server::CMD_NONE);
    server->setWord(Server::STA, Server::STA_NONE);
    //************************************************************
    // načítaj dáta zo serveru
    server->doReadMessage();
    //************************************************************
    // načítaj vstupy z Arduina
    readInputs();
    //************************************************************
    //vykonaj povely od nadradeného servera

    //povel nastavenie teploty
    int setTemp = temp->getSetTemp();
    if(server->getBit(Server::CMD,Server::CMD_TEM_SET)){
        setTemp = server->getWord(Server::STM);
        temp->setSetTemp(setTemp);
    };
    television->setSetTemp( setTemp);
    server->setWord(Server::STM,setTemp);

    //povely pre svetlo
    if(server->getBit(Server::CMD,Server::CMD_LIT_ON)){
        server->setBit(Server::STA,Server::STA_LIT_ON, true);
    };
    if(server->getBit(Server::CMD,Server::CMD_LIT_OFF)){
        server->setBit(Server::STA,Server::STA_LIT_OFF, true);
    };
    if(server->getBit(Server::CMD,Server::CMD_LIT_AUT)){
        server->setBit(Server::STA,Server::STA_LIT_AUT, true);
        server->setBit(Server::STA,Server::STA_LIT_ON, false);
        server->setBit(Server::STA,Server::STA_LIT_OFF, false);
    }
        
    //povely pre garáž
    if(server->getBit(Server::CMD,Server::CMD_GAR_ON)){
        server->setBit(Server::STA,Server::STA_GAR_ON, true);
    };
    if(server->getBit(Server::CMD,Server::CMD_GAR_OFF)){
        server->setBit(Server::STA,Server::STA_GAR_OFF, true);
    };
    if(server->getBit(Server::CMD,Server::CMD_GAR_STOP)){
        server->setBit(Server::STA,Server::STA_GAR_STOP, true);
    };        

    //************************************************************
    // zobraz dáta na displej
    television->drawScreen();
    //************************************************************
    // aktualizovať vystupy Arduina
    writeOutputs();
    //************************************************************
    // odoslať dáta do serveru
    server->doWriteMessage();
    //************************************************************
 
    // pauza pred nasledujucim cyklom
    delay(LOOP_DELAY);
}
