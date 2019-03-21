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
#include "app_config.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <DallasTemperature.h>
#include <MCUFRIEND_kbv.h>
#include <OneWire.h>
#include <easy_code.h>

// funkcia pre nacitanie stavu vystupov
int digitalReadOutputPin(uint8_t pin);

Telegram telegram;

//--------------------------------------------------------------
// kniznica TELEGRAM
//--------------------------------------------------------------
Telegram::Telegram() {
  // iniciovat datove objekty
  memset((byte *)this->getTelegram(), 0, Telegram::MSG_LEN);
  memset((byte *)this->getBuffer(), 0, Telegram::BUF_LEN);
  // preset telegram header
  setByteInTelegram(Telegram::START, Telegram::STX);
  // preset telegram footer
  setByteInTelegram(Telegram::STOP, Telegram::ETX);
}
// prekonvertuje ascii string do bufferu
void Telegram::setBuffer(char *str) {
  strncpy((byte *)this->getBuffer(), str, Telegram::BUF_LEN);
}
// prekonvertuje ascii string do telegramu
void Telegram::setTelegram(char *str) {
  strncpy((byte *)this->getTelegram(), str, Telegram::MSG_LEN);
}
// nastavi Uint8 hodnotu v poli TELEGRAM
void Telegram::setByteInTelegram(int num, byte val) {
  if ((num >= 0) && (num < Telegram::MSG_LEN)) {
    byte *uint8 = (byte *)this->getTelegram();
    uint8[num] = val;
  }
}
// vrati Uint8 hodnotu v poli Telegram
byte Telegram::getByteInTelegram(int num) {
  if ((num >= 0) && (num < Telegram::MSG_LEN)) {
    byte *uint8 = (byte *)this->getTelegram();
    return uint8[num];
  }
  return 0;
}
// nastavi Uint8 hodnotu v poli buffer
void Telegram::setByteInBuffer(int num, byte val) {
  if ((num >= 0) && (num < Telegram::BUF_LEN)) {
    byte *uint8 = (byte *)this->getBuffer();
    uint8[num] = val;
  }
}
// vrati Uint8 hodnotu v poli Buffer
byte Telegram::getByteInBuffer(int num) {
  if ((num >= 0) && (num < Telegram::BUF_LEN)) {
    byte *uint8 = (byte *)this->getBuffer();
    return uint8[num];
  }
  return 0;
}
//--------------------------------------------------------------
// prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool Telegram::isValidTelegram() {
  if (this->msg.stx != Telegram::STX)
    return false;
  if (this->msg.etx != Telegram::ETX)
    return false;
  return true;
}

word Telegram::getUint16(int num) {
  if ((num < 10) && (num >= 0)) {
    word *uint16 = (word *)this->getTelegram();
    uint16 += 2;
    return uint16[num];
  } else {
    return 0;
  }
}

void Telegram::setUint16(int num, word val) {
  if ((num < 10) && (num >= 0)) {
    word *uint16 = (word *)this->getTelegram();
    uint16 += 2;
    uint16[num] = val;
  }
}
// prekonvertuje telegram do char retazca ukonceneho '\x00'
// buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
// buffer - pole znakov kam sa bude konvertovat
// msg - struktura telegramu
// len - velkost struktury telegramu v bajtoch; sizeof(TELEGRAM)
void Telegram::encodeTelegram() {
  byte *b_msg = (byte *)this->getTelegram();
  byte *buf = (byte *)this->getBuffer();
  byte a, b, c;
  int j = 0;

  for (int i = 0; i < Telegram::MSG_LEN; i++) {
    c = b_msg[i];
    a = c & 0x0F;
    b = (c >> 4) & 0x0F;
    buf[j] = (a + Telegram::ADD) & 0xFF;
    j++;
    buf[j] = (b + Telegram::ADD) & 0xFF;
    j++;
  }
  buf[j] = 0x00;
}
// prekonvertuje char retazec ukonceny '\x00' na telegram
// buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
// buffer - pole znakov odkial sa bude konvertovat
// msg - struktura telegramu
// len - velkost struktury telegramu v bajtoch; int len = sizeof(TELEGRAM);
void Telegram::decodeTelegram() {
  byte *b_msg = (byte *)this->getTelegram();
  byte *buf = (byte *)this->getBuffer();
  byte a, b, c;

  for (int i = 0, j = 0; i < Telegram::MSG_LEN; i++) {
    a = (buf[j] - Telegram::ADD) & 0x0F;
    j++;
    b = (buf[j] - Telegram::ADD) & 0x0F;
    j++;
    b_msg[i] = a | (b << 4);
  }
}
//--------------------------------------------------------------
// nastavenia pre teplomery
//--------------------------------------------------------------
// vytvoření instance oneWireDS z knihovny OneWire
OneWire oneWireDS(INPUT_PIN_14); // teplomery sú na pine 14
// vytvoření instance senzoryDS z knihovny DallasTemperature
DallasTemperature senzoryDS(&oneWireDS);
int num_temp = 0; // pocet teplomerov pripojenych
byte addr[MAX_DS1820_SENSORS][8];
char buf[20];
// koľko môže byť DS18S20 teplomerov pripojených
// pole teplôt
// teplota[0] = T1 - teplota okolia
float teplota[MAX_DS1820_SENSORS];
//--------------------------------------------------------------
// nastavenia pre displej
//--------------------------------------------------------------
char string[256];
MCUFRIEND_kbv tft;

void setup_lcd() {
  Serial.println(F("TFT LCD test"));
  tft.reset();

  // Zoznam podporovaných displejov
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9486 LCD driver"));
  } else if (identifier == 0x9488) {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(
        F("If using the Adafruit 3.5\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(
        F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(
        F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9486;
  }
  tft.begin(identifier);

  tft.fillScreen(BLACK);
  unsigned long start = micros();
  // Otočenie obrazovky (o 90 stupňov) (0=0, 1=90, 2=180, 3=270)
  tft.setRotation(3);
}
//--------------------------------------------------------------
// SVETLA
//--------------------------------------------------------------
void kresli_blok_1() { // SVETLA
  // vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(3, 0, 230, 210, 10, WHITE);
  // Text (Stvorec 2) - Svetla
  tft.setCursor(50, 10);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println("Svetla");

  tft.setCursor(68, 65);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println("Spalna");

  tft.setCursor(45, 95);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println("Obyvacka");

  tft.setCursor(75, 125);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println("Garaz");
}
//--------------------------------------------------------------
// kresli jas
//--------------------------------------------------------------
void prekresliBlok1() { // SVETLA
  // prekresli pozadie textu
  tft.fillRect(45, 165, 170, 30, WHITE);
  // nastav parametre textu
  tft.setCursor(45, 170);
  tft.setTextColor(GREY);
  tft.setTextSize(3);
  // zobraz text s hodnotou osvitu
  sprintf(string, "Jas %s", String((int)telegram.msg.w[ATB]).c_str());
  tft.println(string);
}
//--------------------------------------------------------------
// nakresli obdlznik s grafikou garaze
//--------------------------------------------------------------
void kresli_blok_2() { // GARAZ
  // vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(245, 0, 225, 210, 10, WHITE);
  // Text (Stvorec 1) - Garaz
  tft.setCursor(300, 10);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println("Garaz");
}
//--------------------------------------------------------------
// kresli garaze
//--------------------------------------------------------------
void prekresliBlok2() { // GARAZ
  bool isOpened = BITMASK_CHECK_ALL(telegram.msg.w[FDB], FBK_GAR_ON);
  bool isClosed = BITMASK_CHECK_ALL(telegram.msg.w[FDB], FBK_GAR_OFF);
  tft.fillRect(280, 55, 160, 30, WHITE);

  tft.setCursor(280, 55);
  tft.setTextColor(RED);
  tft.setTextSize(3);

  // Garaz otvorena obrazok + text
  if (isOpened) {
    tft.setCursor(290, 55);
    tft.setTextColor(GREEN);
    tft.setTextSize(3);
    tft.println("Otvorena");
    tft.fillRect(267, 100, 180, 100, BLACK);
    tft.fillRect(279, 110, 155, 16, WHITE);
  }
  // Garaz zatvorena text + obrazok
  if (isClosed) {
    tft.setCursor(280, 55);
    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.println("Zatvorena");
    tft.fillRect(267, 100, 180, 100, BLACK);
    tft.fillRect(279, 110, 155, 16, WHITE);
    tft.fillRect(279, 131, 155, 16, WHITE);
    tft.fillRect(279, 152, 155, 16, WHITE);
    tft.fillRect(279, 173, 155, 16, WHITE);
  }
  // Garaz pracuje
  if (!isOpened && !isClosed) {
    tft.setCursor(290, 55);
    tft.setTextColor(MAGENTA);
    tft.setTextSize(3);
    tft.println("PRACUJEM");
    tft.fillRect(267, 100, 180, 100, BLACK);
    tft.fillRect(279, 110, 155, 16, WHITE);
    tft.fillRect(279, 131, 155, 16, WHITE);
  }
}
//--------------------------------------------------------------
// blok s teplotami
//--------------------------------------------------------------
void kresli_blok_3() { // TEPLOTA IZBY

  // vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(3, 220, 474, 100, 10, WHITE);

  // Text (Stvorec 3)- Teplota izby
  tft.setCursor(110, 225);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println("Teplota izby");
}

void prekresliBlok3() { // TEPLOTA IZBY
  tft.fillRect(185, 265, 80, 50, WHITE);

  sprintf(string, "Aktualna  %s C", String((int)telegram.msg.w[ATM]).c_str());
  tft.setCursor(10, 265);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println(string);

  //	tft.fillRect(200, 292, 90, 30, BLUE);
  sprintf(string, "Nastavena %s C", String((int)telegram.msg.w[STM]).c_str());
  tft.setCursor(10, 292);
  tft.setTextColor(MAGENTA);
  tft.setTextSize(3);
  tft.println(string);

  /*tft.setCursor(330,270);
    tft.setTextColor(BLUE); tft.setTextSize(4);
    tft.println("Chladi");*/
  tft.setCursor(292, 275);
  tft.setTextColor(RED);
  tft.setTextSize(3.5);
  tft.println("Prihrieva");
}
//--------------------------------------------------------------
// nacitaj vstupy z Arduina
//--------------------------------------------------------------
void readInputs() {
  //************************************************************
  // načtení hodnoty z analogového pinu
  // Světelný senzor TEMT6000
  // vytvoření proměnných pro výsledky měření
  float analogHodnota = analogRead(analogPin);
  // přepočet analogové hodnoty z celého rozsahu
  //  0-1023 na procentuální rozsah 0-100
  int prepocet = map(analogHodnota, 0, 1023, 0, 100);
  // zapamataj vypocitany osvit
  telegram.msg.w[ATB] = (word)prepocet;
  //************************************************************
  // TEPLOMER
  // načíta všetky teplomery
  senzoryDS.requestTemperatures();
  // uloží teploty do poľa teplota
  for (byte sensor = 0; sensor < num_temp; sensor++) {
    teplota[sensor] = senzoryDS.getTempCByIndex(sensor);
  }
  // zapamataj aktualnu teplotu
  telegram.msg.w[ATM] = teplota[0];
  //************************************************************
  // nacitaj stav tlacitok
  // nastavenie koncovych poloh garaze
  if (digitalRead(INPUT_PIN_15) == HIGH) {          // kontakt otvorena garaz
    BITMASK_CLEAR(telegram.msg.w[FDB], FBK_GAR_ON); // kontakt otvorene true
  } else {
    BITMASK_SET(telegram.msg.w[FDB], FBK_GAR_ON); // kontakt otvorene false
  }
  if (digitalRead(INPUT_PIN_16) == HIGH) {           // kontakt zatvorena garaz
    BITMASK_CLEAR(telegram.msg.w[FDB], FBK_GAR_OFF); // kontakt zatvorene true
  } else {
    BITMASK_SET(telegram.msg.w[FDB], FBK_GAR_OFF); // kontakt zatvorene false
  }
}
//--------------------------------------------------------------
// aktualizovat vystupy Arduina
//--------------------------------------------------------------
void doGarazStop() {
  delay(200);
  digitalWrite(RELE0, HIGH); // relé 0 vypnut
  delay(200);
  digitalWrite(RELE1, HIGH); // relé 1 vypnut
}

void doGarazToOn() {
  delay(200);
  digitalWrite(RELE1, HIGH); // relé 1 vypnut
  delay(200);
  digitalWrite(RELE0, LOW); // relé 0 zapnut
}

void doGarazToOff() {
  delay(200);
  digitalWrite(RELE1, LOW); // relé 1 zapnut
  delay(200);
  digitalWrite(RELE0, LOW); // relé 0 zapnut
}

void writeOutputs() {
  // nastavenie vystupov , relatok
  //************************************************************
  // vyhodnotenie pohybu garaze
  // zisti ktorym smerom sa garaz hybe
  int rele0 = digitalReadOutputPin(RELE0);
  int rele1 = digitalReadOutputPin(RELE1);
  bool onMoveToOpen = BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_GAR_ON);
  bool onMoveToClose = BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_GAR_OFF);
  // ak je poziadavka zatvorit
  if (onMoveToClose) {
    // pohni garaz na zatvorenie
    doGarazToOff();
  }
  // ak je poziadavka zatvorit
  if (onMoveToOpen) {
    if (rele1 == LOW) {
      doGarazStop();
    }
    // pohni garaz na zatvorenie
    doGarazToOn();
  }

  //************************************************************
  // zapnutie svetiel
  // 12V zapnutie LED svetiel
  // ak sa ma svetlo zapnut
  if (BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_LIT_ON)) {
    digitalWrite(RELE2, LOW); // relé 2 zapnut
  }
  // ak sa ma svetlo vypnut
  if (BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_LIT_OFF)) {
    digitalWrite(RELE2, HIGH); // relé 2 vypnut
  }
  // ak sa ma svetlo zapnut automaticky
  if (BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_LIT_AUT)) {
    // vyhodnotit osvit
    int i_val = (int)telegram.msg.w[ATB];
    if (i_val < 20) {
      digitalWrite(RELE2, LOW); // relé 2 zapnut
    } else {
      digitalWrite(RELE2, HIGH); // relé 2 vypnut}
    }
  }
  // odlozit aktualny stav relatok do feedbacku
  if (digitalReadOutputPin(RELE0) == HIGH) {
    BITMASK_CLEAR(telegram.msg.w[FDB], FBK_RELE_0);
  } else {
    BITMASK_SET(telegram.msg.w[FDB], FBK_RELE_0);
  }
  if (digitalReadOutputPin(RELE1) == HIGH) {
    BITMASK_CLEAR(telegram.msg.w[FDB], FBK_RELE_1);
  } else {
    BITMASK_SET(telegram.msg.w[FDB], FBK_RELE_1);
  }
  if (digitalReadOutputPin(RELE2) == HIGH) {
    BITMASK_CLEAR(telegram.msg.w[FDB], FBK_RELE_2);
  } else {
    BITMASK_SET(telegram.msg.w[FDB], FBK_RELE_2);
  }
}
//--------------------------------------------------------------
// vyhodnot prikazy a vykonaj vypocty
//--------------------------------------------------------------
void urobitPrepocty() {
  // ak sa nieco ma zmenit so svetlom
  // vyhodnotit prikazy zo servera
  //*******************************************
  // vyhodnotenie svetla
  word CMD_MASK = CMD_LIT_ON | CMD_LIT_OFF | CMD_LIT_AUT;
  word STA_MASK = STA_LIT_ON | STA_LIT_OFF | STA_LIT_AUT;
  // ak je niektory priznak z povelu pre svetlo aktivny

  // ak sa ma svetlo zapnut
  if (BITMASK_CHECK_ANY(telegram.msg.w[CMD], CMD_LIT_ON)) {
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_LIT_ON);
  }
  // ak sa ma svetlo vypnut
  if (BITMASK_CHECK_ANY(telegram.msg.w[CMD], CMD_LIT_OFF)) {
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_LIT_OFF);
  }
  // ak sa ma svetlo zapnut automaticky
  if (BITMASK_CHECK_ANY(telegram.msg.w[CMD], CMD_LIT_AUT)) {
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_LIT_AUT);
  }
  // koniec poziadaviek pre svetlo
  BITMASK_CLEAR(telegram.msg.w[CMD], CMD_MASK);
  //*******************************************
  // vyhodnotenie pohybu garaze
  CMD_MASK = CMD_GAR_ON | CMD_GAR_OFF | CMD_GAR_STOP;
  STA_MASK = STA_GAR_ON | STA_GAR_OFF | STA_GAR_STOP;
  // zisti ktorym smerom sa garaz hybe
  bool statusOld = BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_MASK);
  // ak chcem aby sa garaz otvorila
  if (BITMASK_CHECK_ANY(telegram.msg.w[CMD], CMD_GAR_ON)) {
    // nech sa zacne hybat k otvoreniu
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_GAR_ON);
  }
  // ak chcem aby sa garaz zatvorila
  if (BITMASK_CHECK_ANY(telegram.msg.w[CMD], CMD_GAR_OFF)) {
    // nech sa zacne hybat k zatvoreniu
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_GAR_OFF);
  }
  // ak chcem aby sa garaz zatvorila
  if (BITMASK_CHECK_ANY(telegram.msg.w[CMD], CMD_GAR_STOP)) {
    // nech sa zastavi
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_GAR_STOP);
  }
  // koniec poziadaviek pre garaz
  BITMASK_CLEAR(telegram.msg.w[CMD], CMD_MASK);
  //*******************************************
  // vyhodnotenie koncovych poloh garaze
  // zisti ktorym smerom sa garaz hybe
  word statusNew = BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_MASK);
  if (statusOld != statusNew) {
    doGarazStop();
  }
  bool onMoveToOpen = BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_GAR_ON);
  bool onMoveToClose = BITMASK_CHECK_ALL(telegram.msg.w[STA], STA_GAR_OFF);
  bool isOpened = BITMASK_CHECK_ALL(telegram.msg.w[FDB], FBK_GAR_ON);
  bool isClosed = BITMASK_CHECK_ALL(telegram.msg.w[FDB], FBK_GAR_OFF);

  if (onMoveToOpen && isOpened) {
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_GAR_STOP);
    doGarazStop();
  }
  if (onMoveToClose && isClosed) {
    BITMASK_CLEAR(telegram.msg.w[STA], STA_MASK);
    BITMASK_SET(telegram.msg.w[STA], STA_GAR_STOP);
    doGarazStop();
  }
}
//--------------------------------------------------------------
// nacitaj data zo serveru
//--------------------------------------------------------------
void readFromServer() {
  Telegram new_msg;
  // ak su prijate data zo servera, nacita ich
  if (Serial.available() >= Telegram::BUF_LEN) {
    for (int i = 0; i < Telegram::BUF_LEN; i++) {
      byte c = Serial.read();
      new_msg.setByteInBuffer(i, c);
    }
    while (Serial.available()) {
      byte c = Serial.read();
    }
    //    Serial.println((char *)new_msg.getBuffer());
    // dekoduj spravu od servera
    new_msg.decodeTelegram();
    // ak je sprava validna
    if (new_msg.isValidTelegram()) {
      //  Serial.println("arduino rx is valid\n");
      // prekopiruj cmd - prikazy zo servera pre Arduino
      telegram.msg.w[CMD] = new_msg.msg.w[CMD];
      //*******************************************
      // vyhodnotenie nastavenia teploty
      // ak sa ma svetlo zapnut
      if (BITMASK_CHECK_ALL(telegram.msg.w[CMD], CMD_TEM_SET)) {
        // prekopiruj hodnotu setpointu
        telegram.msg.w[STM] = new_msg.msg.w[STM];
      }
    }
  }
}
//--------------------------------------------------------------
// odoslat data do serveru
//--------------------------------------------------------------
void writeToServer() {
  // zakoduj spravu pre server
  telegram.encodeTelegram();
  // odosli data po seriovej linke
  Serial.println((char *)telegram.getBuffer());
}
//--------------------------------------------------------------
// nacitaj aktualny stav vystupu
//--------------------------------------------------------------
int digitalReadOutputPin(uint8_t pin) {
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  if (port == NOT_A_PIN)
    return LOW;

  return (*portOutputRegister(port) & bit) ? HIGH : LOW;
}
//--------------------------------------------------------------
// nastavenie Arduina
//--------------------------------------------------------------
void setup() {
  //************************************************************
  // commandy
  telegram.msg.w[STM] = 25; // ziadne prikazy z nadradeneho servera
  telegram.msg.w[CMD] = 0;  // ziadne prikazy z nadradeneho servera
  //************************************************************
  // nastavenie vstupov
  pinMode(INPUT_PIN_15, INPUT); // kontakt otvorena garaz
  pinMode(INPUT_PIN_16, INPUT); // kontakt zatvorena garaz
  // nastavenie vystupov , relatok
  pinMode(RELE0, OUTPUT); // rezerva
  digitalWrite(RELE0, HIGH);
  pinMode(RELE1, OUTPUT); // rezerva
  digitalWrite(RELE1, HIGH);
  pinMode(RELE2, OUTPUT); // 12V rezerva
  digitalWrite(RELE2, HIGH);
  pinMode(RELE3, OUTPUT); // 12V zapnutie LED svetiel
  digitalWrite(RELE3, HIGH);
  pinMode(RELE4, OUTPUT); // 12V nastavenie smeru pohybu motora garaze
  digitalWrite(RELE4, HIGH);
  pinMode(RELE5, OUTPUT); // 5V zapnutie motora garaze
  digitalWrite(RELE5, HIGH);
  //************************************************************
  // začni komunikovať s teplomermi
  senzoryDS.begin();
  for (int i = 0; i < MAX_DS1820_SENSORS; i++) {
    if (!oneWireDS.search(addr[i])) {
      oneWireDS.reset_search();
      delay(250);
      break;
    }
    num_temp++; // počet teplomerov zvýš o 1
    delay(500);
  }
  //************************************************************
  // zahájení komunikace po sériové lince
  // rychlostí 115200 baud
  Serial.begin(115200);
  //************************************************************
  // nastavi displej
  setup_lcd();
  //************************************************************
  kresli_blok_1();
  //************************************************************
  prekresliBlok1();
  kresli_blok_2();
  prekresliBlok2();
  kresli_blok_3();
  prekresliBlok3();
}
//--------------------------------------------------------------
// hlavna slucka programu
//--------------------------------------------------------------
void loop() {
  //************************************************************
  // zapametaj predosly stav periferii
  TELEGRAM last_msg;
  memcpy(&last_msg, &telegram.msg, sizeof(telegram.msg));
  //************************************************************
  // nacitaj data zo serveru
  readFromServer();
  //************************************************************
  // nacitaj vstupy z Arduina
  readInputs();
  //************************************************************
  // vyhodnot prikazy a vykonaj vypocty
  urobitPrepocty();
  //************************************************************
  // zobrazit data na displej
  // vyhodnotenie jasu
  if (last_msg.w[ATB] != telegram.msg.w[ATB]) {
    prekresliBlok1();
  }
  // vyhodnotenie garazovych dveri
  // vyhodnotenie garaze
  bool isOpened = BITMASK_CHECK_ALL(telegram.msg.w[FDB], FBK_GAR_ON);
  bool isClosed = BITMASK_CHECK_ALL(telegram.msg.w[FDB], FBK_GAR_OFF);
  bool wasOpened = BITMASK_CHECK_ALL(last_msg.w[FDB], FBK_GAR_ON);
  bool wasClosed = BITMASK_CHECK_ALL(last_msg.w[FDB], FBK_GAR_OFF);

  if ((isOpened != wasOpened) || (isClosed != wasClosed)) {
    prekresliBlok2();
  }
  // vyhodnotenie teploty v izbe
  if ((last_msg.w[STM] != telegram.msg.w[STM]) ||
      (last_msg.w[ATM] != telegram.msg.w[ATM])) {
    prekresliBlok3();
  }
  //************************************************************
  // aktualizovat vystupy Arduina
  writeOutputs();
  //************************************************************
  // odoslat data do serveru
  writeToServer();
  //************************************************************
  telegram.msg.w[CMD] = 0; // ziadne prikazy z nadradeneho servera
  // pauza pred nasledujucim cyklom
  delay(500);
}
