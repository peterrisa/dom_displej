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
//  B_VALUE b_0; // 2 bytes -- setpoint teplota
//  B_VALUE b_1; // 2 bytes -- skutocna teplota
//  B_VALUE b_2; // 2 bytes -- aktualny osvit
//  B_VALUE b_3; // 2 bytes
//  B_VALUE b_4; // 2 bytes -- DEBUG tx prikazy
//  B_VALUE b_5; // 2 bytes -- DEBUG tx setpoint
//  B_VALUE b_6; // 2 bytes -- DEBUG tx setpoint teplota
//  B_VALUE b_7; // 2 bytes -- DEBUG
//  B_VALUE b_8; // 2 bytes -- feedbacky
//  B_VALUE b_9; // 2 bytes -- prikazy
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

Telegram telegram;

Telegram::Telegram() {
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

//-----------------------------------------------------------
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
  tft.setRotation(1);
}

//--------------------------------------------------------------

// Začiatok kódu

// SVETLA
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
// kresli jas
void funk1() { // SVETLA
               // prekresli pozadie textu
  tft.fillRect(45, 165, 170, 30, WHITE);
  // nastav parametre textu
  tft.setCursor(45, 170);
  tft.setTextColor(GREY);
  tft.setTextSize(3);
  // zobraz text s hodnotou osvitu
  sprintf(string, "Jas %s", String((int)telegram.msg.b_2.b).c_str());
  tft.println(string);
}

//------------------------------------------------------------------
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

void funk2() { // GARAZ
  tft.fillRect(280, 55, 160, 30, WHITE);

  tft.setCursor(280, 55);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  /*tft.println("Zatvorena");
  tft.setCursor(290, 55);
  tft.setTextColor(GREEN);  tft.setTextSize(3);
  tft.println("Otvorena");*/

  // Garaz otvorena obrazok + text
  if (!telegram.msg.b_8.v.b_03) {
    tft.setCursor(290, 55);
    tft.setTextColor(GREEN);
    tft.setTextSize(3);
    tft.println("Otvorena");
    tft.fillRect(267, 100, 180, 100, BLACK);
    tft.fillRect(279, 110, 155, 16, WHITE);
  } else {
    // tft.fillRect(279, 110, 1, 1, WHITE);
    tft.setCursor(290, 55);
    /*	tft.setTextColor(MAGENTA);  tft.setTextSize(3);
            tft.println("PRACUJEM");
            tft.fillRect(267, 100, 180, 100, BLACK);
            tft.fillRect(279, 110, 155, 16, WHITE);
            tft.fillRect(279, 131, 155, 16, WHITE);*/
  }

  // Garaz zatvorena text + obrazok
  if (!telegram.msg.b_8.v.b_04) {
    tft.setCursor(280, 55);
    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.println("Zatvorena");
    tft.fillRect(267, 100, 180, 100, BLACK);
    tft.fillRect(279, 110, 155, 16, WHITE);
    tft.fillRect(279, 131, 155, 16, WHITE);
    tft.fillRect(279, 152, 155, 16, WHITE);
    tft.fillRect(279, 173, 155, 16, WHITE);
  } else {
    // tft.fillRect(279, 11, 1, 1, WHITE);
    tft.setCursor(290, 55);
    /*tft.setTextColor(MAGENTA);  tft.setTextSize(3);
    tft.println("PRACUJEM");
    tft.fillRect(267, 100, 180, 100, BLACK);
    tft.fillRect(279, 110, 155, 16, WHITE);
    tft.fillRect(279, 131, 155, 16, WHITE);*/
  }

  // Garaz pracuje
  if (!telegram.msg.b_8.v.b_03 || !telegram.msg.b_8.v.b_04) {
    tft.fillRect(279, 110, 1, 1, WHITE);
  } else {
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

void funk3() { // TEPLOTA IZBY
  tft.fillRect(185, 265, 80, 50, WHITE);

  sprintf(string, "Aktualna  %s C", String((int)telegram.msg.b_1.b).c_str());
  tft.setCursor(10, 265);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println(string);

  //	tft.fillRect(200, 292, 90, 30, BLUE);
  sprintf(string, "Nastavena %s C", String((int)telegram.msg.b_0.b).c_str());
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

void setup() {
  // nastavenie datovej struktury
  telegram.msg.stx = Telegram::STX; // povinna konstanta
  telegram.msg.etx = Telegram::ETX; // povinna konstanta
  telegram.msg.b_0.b = 10;          // prednastaneva teplota
  telegram.msg.b_1.b = 0;           // aktualna teplota
  telegram.msg.b_2.b = 0;           // detektor svetla
  // nastavenie koncovych poloh garaze
  telegram.msg.b_8.v.b_03 = false; // kontakt otvorene
  telegram.msg.b_8.v.b_04 = false; // kontakt zatvorene
  // koncove polohy zaluzie
  telegram.msg.b_8.v.b_05 = false; // kontakt otvorene
  telegram.msg.b_8.v.b_06 = false; // kontakt zatvorene
  // kurenie
  telegram.msg.b_8.v.b_07 = false; // kurenie zapnute
  telegram.msg.b_8.v.b_08 = false; // chladenie zapnute
  telegram.msg.b_8.v.b_09 = false; // ochrana zapnuta
  // relatka
  telegram.msg.b_8.v.b_10 = false; // rele_0
  telegram.msg.b_8.v.b_11 = true;  // rele_1
  telegram.msg.b_8.v.b_12 = false; // rele_2
  telegram.msg.b_8.v.b_13 = true;  // rele_3
  telegram.msg.b_8.v.b_14 = false; // rele_4
  telegram.msg.b_8.v.b_15 = true;  // rele_5
  // LED
  telegram.msg.b_8.v.b_00 = false; // zapnutie svetla
  // commandy
  telegram.msg.b_6.b = 30; // ziadne prikazy z nadradeneho servera
  telegram.msg.b_9.b = 0;  // ziadne prikazy z nadradeneho servera
  // nastvanie vstupov
  pinMode(INPUT_PIN_15, INPUT);
  pinMode(INPUT_PIN_16, INPUT);
  // nastavenie LED
  pinMode(LED_18, OUTPUT);
  // nastavenie vystupov , relatok
  pinMode(RELE0_PIN_23, OUTPUT);
  pinMode(RELE1_PIN_25, OUTPUT);
  pinMode(RELE2_PIN_27, OUTPUT);
  pinMode(RELE3_PIN_29, OUTPUT);
  pinMode(RELE4_PIN_31, OUTPUT);
  pinMode(RELE5_PIN_33, OUTPUT);
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

  // zahájení komunikace po sériové lince
  // rychlostí 115200 baud
  Serial.begin(115200);

  // nastavi displej
  setup_lcd();
  kresli_blok_1();
  funk1();
  kresli_blok_2();
  funk2();
  kresli_blok_3();
  funk3();
}
// nacitat vstupy Arduina
void readInputs() {
  // načtení hodnoty z analogového pinu
  // Světelný senzor TEMT6000
  // vytvoření proměnných pro výsledky měření
  float analogHodnota = analogRead(analogPin);
  // přepočet analogové hodnoty z celého rozsahu
  //  0-1023 na procentuální rozsah 0-100
  int prepocet = map(analogHodnota, 0, 1023, 0, 100);
  telegram.msg.b_2.b = (word)prepocet;

  // TEPLOMER
  // načíta všetky teplomery
  senzoryDS.requestTemperatures();
  // uloží teploty do poľa teplota
  for (byte sensor = 0; sensor < num_temp; sensor++) {
    teplota[sensor] = senzoryDS.getTempCByIndex(sensor);
  }
  // priradi aktualnu teplotu do telegramu
  telegram.msg.b_1.b = teplota[0];

  // nacitaj stav tlacitok
  // nastavenie koncovych poloh garaze
  if (digitalRead(INPUT_PIN_15) == HIGH) {
    telegram.msg.b_8.v.b_03 = true; // kontakt otvorene true
  } else {
    telegram.msg.b_8.v.b_03 = false; // kontakt otvorene false
  }
  if (digitalRead(INPUT_PIN_16) == HIGH) {
    telegram.msg.b_8.v.b_04 = true; // kontakt zatvorene true
  } else {
    telegram.msg.b_8.v.b_04 = false; // kontakt zatvorene false
  }
}

void writeOutputs() {
  if (telegram.msg.b_8.v.b_00) {
    digitalWrite(LED_18, HIGH); // LED zapnute
  } else {
    digitalWrite(LED_18, LOW); // LED zapnute
  }
  if (!telegram.msg.b_8.v.b_10) {
    digitalWrite(RELE0_PIN_23, HIGH); // relé0 rozopnute
  } else {
    digitalWrite(RELE0_PIN_23, LOW); // relé0 zapnute
  }

  if (!telegram.msg.b_8.v.b_11) {
    digitalWrite(RELE1_PIN_25, HIGH); // relé1 rozopnute
  } else {
    digitalWrite(RELE1_PIN_25, LOW); // relé1 zapnute
  }

  if (!telegram.msg.b_8.v.b_12) {
    digitalWrite(RELE2_PIN_27, HIGH); // relé1 rozopnute
  } else {
    digitalWrite(RELE2_PIN_27, LOW); // relé1 zapnute
  }

  if (!telegram.msg.b_8.v.b_13) {
    digitalWrite(RELE3_PIN_29, HIGH); // relé1 rozopnute
  } else {
    digitalWrite(RELE3_PIN_29, LOW); // relé1 zapnute
  }

  if (!telegram.msg.b_8.v.b_14) {
    digitalWrite(RELE4_PIN_31, HIGH); // relé1 rozopnute
  } else {
    digitalWrite(RELE4_PIN_31, LOW); // relé1 zapnute
  }

  if (!telegram.msg.b_8.v.b_15) {
    digitalWrite(RELE5_PIN_33, HIGH); // relé5 rozopnute
  } else {
    digitalWrite(RELE5_PIN_33, LOW); // relé5 zapnute
  }
}

void urobitPrepocty() {
  // vyhodnotit osvit
  int i_val = (int)telegram.msg.b_2.b;

  // zapnutie LED
  if (i_val < 20) {
    telegram.msg.b_8.v.b_00 = true;
  } else {
    telegram.msg.b_8.v.b_00 = false;
  }
  // prikazy z nadradeneho systemu
  if (telegram.msg.b_6.b) {
    telegram.msg.b_0.b = telegram.msg.b_6.b;
  }
  if (telegram.msg.b_9.b) {
    // tu bude logika pre riadenie motorov
  }
}
// nacitat data zo serveru
void readFromServer() {
  Telegram new_msg;

  if (Serial.available() >= Telegram::BUF_LEN) {
    for (int i = 0; i < Telegram::BUF_LEN; i++) {
      byte c = Serial.read();
      new_msg.setByteInBuffer(i, c);
    }
    while (Serial.available()) {
      byte c = Serial.read();
    }
    //    Serial.println((char *)new_msg.getBuffer());
    new_msg.decodeTelegram();

    if (new_msg.isValidTelegram()) {
      //  Serial.println("arduino rx is valid\n");
      // prekopiruj cmd - prikazy zo servera pre Arduino
      telegram.msg.b_9.b = new_msg.msg.b_9.b;
      telegram.msg.b_6.b = new_msg.msg.b_6.b;

      // DEBUG
      telegram.msg.b_4.b = new_msg.msg.b_9.b;
      telegram.msg.b_5.b = new_msg.msg.b_8.b;
      telegram.msg.b_7.b = new_msg.msg.b_6.b;
    }
  }
}
// zapisat data na server
void writeToServer() {
  telegram.encodeTelegram();
  Serial.println((char *)telegram.getBuffer());
}

void loop() {
  // zapametaj predosly stav periferii
  TELEGRAM last_msg;
  memcpy(&last_msg, &telegram.msg, sizeof(telegram.msg));
  // nacitaj data zo serioveho portu
  readFromServer();
  // nacita data zo vstupov Arduina
  readInputs();
  // vyhodnot data, urob prepocty
  urobitPrepocty();
  //-----------------------------------------------
  // zobrazit data na displej
  //-----------------------------------------------

  // vyhodnotenie jasu
  if (last_msg.b_2.b != telegram.msg.b_2.b) {
    funk1();
  }
  // vyhodnotenie garazovych dveri
  if ((last_msg.b_8.v.b_03 != telegram.msg.b_8.v.b_03) ||
      (last_msg.b_8.v.b_04 != telegram.msg.b_8.v.b_04)) {
    funk2();
  }
  // vyhodnotenie teploty v izbe
  if ((last_msg.b_0.b != telegram.msg.b_0.b) ||
      (last_msg.b_1.b != telegram.msg.b_1.b)) {
    funk3();
  }
  // odoslat data do periferii
  writeOutputs();
  // odoslat data do serveru
  writeToServer();
  // pauza
  delay(1000);
}
