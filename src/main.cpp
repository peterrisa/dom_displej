
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
//This Connection Only for UNO, Do not support Mega2560
//because they use different Hardware-SPI Pins
//SD_SS    connects to digital pin 10
//SD_DI    connects to digital pin 11
//SD_DO    connects to digital pin 12
//SD_SCK   connects to digital pin 13
//--------------------------------------------------------------



//Knižnice

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library


#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3   // Chip Select goes to Analog 3
#define LCD_CD A2  // Command/Data goes to Analog 2
#define LCD_WR A1  // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

//--------------------------------------------------------------
// Farby
#define	BLACK    0x0000
#define	BLUE     0x001F
#define	RED      0xF800
#define	GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define GREY     0x7BCF
#define BLUGREEN 0x0B74
#define WHITE    0xFFFF
//--------------------------------------------------------------


char string[256];

struct Data{
  //aktualna teplota
  float temp_act;
  // prednastaneva teplota
  float temp_set;
  // detektor svetla
  float svetla;
  // koncove polohy garaz
  bool garaz_o;
  bool garaz_z;
  // koncove polohy zaluzie
  bool zaluzie_o;
  bool zaluzie_z;
  // kurenie
  bool kurenie_zap;
  bool chladenie_zap;
  bool ochrana_zap;
} data;


MCUFRIEND_kbv tft;

//--------------------------------------------------------------
void setup_lcd (){
  Serial.println(F("TFT LCD test"));
  tft.reset();

  //Zoznam podporovaných displejov
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    /*Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9595) {
    Serial.println(F("Found HX8347-I LCD driver"));
  } else if(identifier == 0x4747) {
    Serial.println(F("Found HX8347-D LCD driver"));
  } else if(identifier == 0x8347) {
    Serial.println(F("Found HX8347-A LCD driver"));
  }else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  }else if(identifier == 0x7783) {
    Serial.println(F("Found ST7781 LCD driver"));
  }else if(identifier == 0x8230) {
    Serial.println(F("Found UC8230 LCD driver"));
  }else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101){
      identifier=0x9341;
      Serial.println(F("Found 0x9341 LCD driver"));
  }else if(identifier==0x7793){
       Serial.println(F("Found ST7793 LCD driver"));
  }else if(identifier==0xB509){
       Serial.println(F("Found R61509 LCD driver"));
  }else if(identifier==0x9486){
   Serial.println(F("Found ILI9488 LCD driver"));
  }else {*/
       Serial.println(F("Found ILI9486 LCD driver"));
  }else if(identifier==0x9488){

    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 3.5\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9486;
  }
  tft.begin(identifier);

     tft.fillScreen(BLACK);
  unsigned long start = micros();
  // Otočenie obrazovky (o 90 stupňov) (0=0, 1=90, 2=180, 3=270)
  tft.setRotation(1);
}
//--------------------------------------------------------------



// Začiatok kódu

//SVETLA
void funk1(){  //SVETLA
  //vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(3, 0, 230, 210, 10, WHITE);
   //Text (Stvorec 2) - Svetla
  tft.setCursor(50,10);
  tft.setTextColor(BLACK); tft.setTextSize(4);
  tft.println("Svetla");


  tft.setCursor(45,170);
  tft.setTextColor(GREY); tft.setTextSize(3);
sprintf(string, "Jas %s", String(data.svetla).c_str());
tft.println(string);
  tft.setCursor(68,65);
  tft.setTextColor(BLACK); tft.setTextSize(3);
  tft.println("Spalna");

tft.setCursor(45,95);
  tft.setTextColor(BLACK); tft.setTextSize(3);
  tft.println("Obyvacka");


  tft.setCursor(75,125);
  tft.setTextColor(BLACK); tft.setTextSize(3);
  tft.println("Garaz");

}

void funk2(){ //GARAZ

  //vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(245, 0, 225, 210, 10, WHITE);

  //Vykreslenie hranatého štvorca - garaz
  //(osa X, osa Y, rozmery X, Rozmery Y, farba)

  // Obrazok garaze - zatvorena
  tft.fillRect(267, 100, 180, 100, BLACK);
  tft.fillRect(279, 110, 155, 16, WHITE);
  tft.fillRect(279, 131, 155, 16, WHITE);
  tft.fillRect(279, 152, 155, 16, WHITE);
  tft.fillRect(279, 173, 155, 16, WHITE);

  /* // Obrazok garaze - otvorena
  tft.fillRect(267, 100, 180, 100, BLACK);
  tft.fillRect(279, 110, 155, 16, WHITE);*/

//Text (Stvorec 1) - Garaz
  tft.setCursor(300, 10);
  tft.setTextColor(BLACK);  tft.setTextSize(4);
  tft.println("Garaz");
  tft.setCursor(280, 55);
  tft.setTextColor(RED);  tft.setTextSize(3);
  tft.println("Zatvorena");
  /*tft.setCursor(290, 55);
  tft.setTextColor(GREEN);  tft.setTextSize(3);
  tft.println("Otvorena");*/

}

void funk3(){ //TEPLOTA IZBY

  //vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(3, 220, 474, 100, 10, WHITE);


  //Text (Stvorec 3)- Teplota izby
  tft.setCursor(110,225);
  tft.setTextColor(BLACK); tft.setTextSize(4);
  tft.println("Teplota izby");

  sprintf(string, "Aktualna  %s C", String(data.temp_act).c_str());
  tft.setCursor(10,265);
  tft.setTextColor(BLACK); tft.setTextSize(3);
  tft.println(string);
  Serial.println(string);

  sprintf(string,"Nastavena %s C", String(data.temp_set).c_str());

tft.setCursor(10,292);
  tft.setTextColor(MAGENTA); tft.setTextSize(3);
  tft.println(string);
  Serial.println(string);

tft.setCursor(330,270);
  tft.setTextColor(BLUE); tft.setTextSize(4);
  tft.println("Chladi");

/*tft.setCursor(350,270);
  tft.setTextColor(RED); tft.setTextSize(4);
  tft.println("Kuri");*/
}


// Světelný senzor TEMT6000

// nastavení čísla prepojovanieho pinu

  #define analogPin A6
// vytvoření proměnných pro výsledky měření
float analogHodnota;
int prepocet;

void setup() {

  //nastavenie datovej struktury
  data.temp_act = 20.0;
  // prednastaneva teplota
  data.temp_set = 10.0;
  // prednstsvrnir dvrtls
  data.svetla = 0.0;
  //nastavenie koncovych poloh garaze
  data.garaz_o = true;
  data.garaz_z = false;
  // koncove polohy zaluzie
  data.zaluzie_o = true;
  data.zaluzie_z = false;
  // kurenie
  data.kurenie_zap = false;
  data.chladenie_zap = false;
  data.ochrana_zap = false;


  // zahájení komunikace po sériové lince
  // rychlostí 9600 baud
  Serial.begin(9600);


  Serial.println(string);
  //nastavi displej
  setup_lcd();
  funk1();
  funk2();
  funk3();

}

void  nacitaj_data(){
    // načtení hodnoty z analogového pinu
  analogHodnota = analogRead(analogPin);


  }

void  odoslat_data(){

  }

void  urobit_prepocty(){
   // přepočet analogové hodnoty z celého rozsahu
  //  0-1023 na procentuální rozsah 0-100
  prepocet = map(analogHodnota, 0, 1023, 0, 100);

  data.svetla = analogHodnota / 1023 * 100;
  }

void loop(){
  struct Data predosle_data;
  memcpy(&predosle_data, &data, sizeof(predosle_data));


// nacita data z periferii do struktury data
  nacitaj_data();

// vyhodnot data, urob prepocty
  urobit_prepocty();

// zobrazit data na displej
  // výpis načtených dat po sériové lince
  Serial.print("Načítaná hodnota: ");
  Serial.print(analogHodnota);
  Serial.print(", v percentách: ");
  Serial.print(prepocet);
  if (prepocet > 50) {
    Serial.println(" | V okolí je dostatok svetla.");
  }
  else {
    Serial.println(" | V okolí je málo svetla.");
  }


  if(predosle_data.svetla != data.svetla){
    funk1();
  }
  if((predosle_data.garaz_o != data.garaz_o) || (predosle_data.garaz_z != data.garaz_z)){
    funk2();
  }
  if((predosle_data.temp_act != data.temp_act) || (predosle_data.temp_set != data.temp_set)){
    funk3();
  }

// odoslat data do periferii
  odoslat_data();

// pauza
  delay(500);
}
