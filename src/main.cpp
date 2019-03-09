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

#include <easy_code.h>
#include "app_config.h"

Telegram telegram;

Telegram::Telegram(){
	//preset telegram header
	setByteInTelegram(Telegram::START,Telegram::STX);
	//preset telegram footer
	setByteInTelegram(Telegram::STOP,Telegram::ETX);
}

void Telegram::setBuffer(char *str){
	byte *src = str;
	byte *dst = (byte *)&this.getBuffer()[0];
	for(int i=0; i<Telegram.BUF_LEN; i++){
		if(*src != 0x00){
			*dst = *src;
			dst++;
			src++;
		}else{
			break;
		}
	}

void Telegram::setTelegram(char *str){
	byte *src = str;
	byte *dst = (byte *)&this.getTelegram()[0];
	for(int i=0; i<Telegram.MSG_LEN; i++){
		if(*src != 0x00){
			*dst = *src;
			dst++;
			src++;
		}else{
			break;
		}
	}

	var telegram = new DataView(this.getTelegram(), 0, Telegram.MSG_LEN);

		const buf = Buffer.from(str, 'ascii');
		var i = 0;
		for (const b of buf) {
			if(i < Telegram.MSG_LEN){
				telegram.setUint8(i, b);
			}
			i++;
		}
}

//nastavi Uint8 hodnotu v poli TELEGRAM
void Telegram::setByteInTelegram(int num, byte val){
	if( (num >= 0) && (num < Telegram::MSG_LEN)){
			byte *uint8 = (byte *)&this.getTelegram()[0];
			uint8[num] = val;
	}
}
//vrati Uint8 hodnotu v poli Telegram
byte Telegram::getByteInTelegram(int num){
	if( (num >= 0) && (num < Telegram::MSG_LEN)){
			byte *uint8 = (byte *)&this.getTelegram()[0];
		return uint8[num];
	}
	return 0;
}

// nastavi Uint8 hodnotu v poli buffer
void Telegram::setByteInBuffer(int num, byte val){
	if( (num >= 0) && (num < Telegram::BUF_LEN)){
			byte *uint8 = (byte *)&this.getBuffer()[0];
			uint8[num] = val;
		}
}
//vrati Uint8 hodnotu v poli Buffer
byte Telegram::getByteInBuffer(int num){
	if( (num >= 0) && (num < Telegram::BUF_LEN)){
			byte *uint8 = (byte *)&this.getBuffer()[0];
			return uint8[num];
		}
	return 0;
}

//-----------------------------------------------------------
//prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool Telegram::isValidTelegram(){
	if(msg.stx != Telegram::STX)
		return false;
	if(msg.etx != Telegram::ETX)
		return false;
	return true;
}


	word Telegram::getUint16(num){
		if( (num < 10) && (num >= 0)){
			word *uint16 = (word *)&this.getTelegram()[2];
				return uint16[num];
			}else{
				return undefined;
			}
	}

	void Telegram::void setUint16(int num, word val){
		if( (num < 10) && (num >= 0)){
			word *uint16 = (word *)&this.getTelegram()[2];
			return uint16[num] = val;
		}else{
			return;
		}

}
//prekonvertuje telegram do char retazca ukonceneho '\x00'
//buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
//buffer - pole znakov kam sa bude konvertovat
//msg - struktura telegramu
//len - velkost struktury telegramu v bajtoch; sizeof(TELEGRAM)
void Telegram::encodeTelegram(){
	byte *b_msg = (byte *)&this->msg;
	byte *buf = this->buffer;
  byte a,b,c;

	for( int i=0, j=0; i < Telegram::MSG_LEN; i++ ){
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

//prekonvertuje char retazec ukonceny '\x00' na telegram
//buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
//buffer - pole znakov odkial sa bude konvertovat
//msg - struktura telegramu
//len - velkost struktury telegramu v bajtoch; int len = sizeof(TELEGRAM);
void Telegram::


decodeTelegram(){
  byte *b_msg = (byte *)&this->msg;
  byte *buf = this->buffer;
  byte a,b,c;

	for( var i=0, j=0; i < Telegram::MSG_LEN; i++ ){
		a = (buf[j] - Telegram::ADD) & 0x0F;
		j++;
		b = (buf[j] - Telegram::ADD) & 0x0F;
		j++;
		b_msg[i] = a | (b << 4);
	}
}

//--------------------------------------------------------------
char string[256];

MCUFRIEND_kbv tft;

void setup_lcd (){
  Serial.println(F("TFT LCD test"));
  tft.reset();

  //Zoznam podporovaných displejov
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
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
void kresli_blok_1(){  //SVETLA
  //vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
	tft.fillRoundRect(3, 0, 230, 210, 10, WHITE);
   //Text (Stvorec 2) - Svetla
  tft.setCursor(50,10);
  tft.setTextColor(BLACK); tft.setTextSize(4);
  tft.println("Svetla");

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

void funk1(){  //SVETLA

tft.fillRect(45, 165, 160, 30, WHITE);


  tft.setCursor(45,170);
  tft.setTextColor(GREY); tft.setTextSize(3);
sprintf(string, "Jas %s", String(msg.v_3.v).c_str());
tft.println(string);


}

//------------------------------------------------------------------
void kresli_blok_2(){ //GARAZ

  //vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(245, 0, 225, 210, 10, WHITE);

//Text (Stvorec 1) - Garaz
  tft.setCursor(300, 10);
  tft.setTextColor(BLACK);  tft.setTextSize(4);
  tft.println("Garaz");

}

void funk2(){ //GARAZ
	tft.fillRect(280, 55, 160, 30, WHITE);

  tft.setCursor(280, 55);
  tft.setTextColor(RED);  tft.setTextSize(3);
  /*tft.println("Zatvorena");
  tft.setCursor(290, 55);
  tft.setTextColor(GREEN);  tft.setTextSize(3);
  tft.println("Otvorena");*/


	// Garaz otvorena obrazok + text
		if(!msg.b_1.v.b_00) {
			tft.setCursor(290, 55);
			tft.setTextColor(GREEN);  tft.setTextSize(3);
			tft.println("Otvorena");
			tft.fillRect(267, 100, 180, 100, BLACK);
			tft.fillRect(279, 110, 155, 16, WHITE);
		}else{
			//tft.fillRect(279, 110, 1, 1, WHITE);
			tft.setCursor(290, 55);
		/*	tft.setTextColor(MAGENTA);  tft.setTextSize(3);
			tft.println("PRACUJEM");
			tft.fillRect(267, 100, 180, 100, BLACK);
			tft.fillRect(279, 110, 155, 16, WHITE);
			tft.fillRect(279, 131, 155, 16, WHITE);*/

		}

			//Garaz zatvorena text + obrazok
			if(!msg.b_1.v.b_01){
				tft.setCursor(280, 55);
				tft.setTextColor(RED);  tft.setTextSize(3);
				tft.println("Zatvorena");
				tft.fillRect(267, 100, 180, 100, BLACK);
				tft.fillRect(279, 110, 155, 16, WHITE);
				tft.fillRect(279, 131, 155, 16, WHITE);
				tft.fillRect(279, 152, 155, 16, WHITE);
				tft.fillRect(279, 173, 155, 16, WHITE);
				}else{
				//tft.fillRect(279, 11, 1, 1, WHITE);
				tft.setCursor(290, 55);
				/*tft.setTextColor(MAGENTA);  tft.setTextSize(3);
				tft.println("PRACUJEM");
				tft.fillRect(267, 100, 180, 100, BLACK);
				tft.fillRect(279, 110, 155, 16, WHITE);
				tft.fillRect(279, 131, 155, 16, WHITE);*/
			}

			//Garaz pracuje
			if(!msg.b_1.v.b_01 || !msg.b_1.v.b_00){
				tft.fillRect(279, 110, 1, 1, WHITE);
			}else{
				tft.setCursor(290, 55);
				tft.setTextColor(MAGENTA);  tft.setTextSize(3);
				tft.println("PRACUJEM");
				tft.fillRect(267, 100, 180, 100, BLACK);
				tft.fillRect(279, 110, 155, 16, WHITE);
				tft.fillRect(279, 131, 155, 16, WHITE);

			}

}

//--------------------------------------------------------------
void kresli_blok_3(){ //TEPLOTA IZBY

  //vykreslenie oblého štvorca
  //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
  tft.fillRoundRect(3, 220, 474, 100, 10, WHITE);

  //Text (Stvorec 3)- Teplota izby
  tft.setCursor(110,225);
  tft.setTextColor(BLACK); tft.setTextSize(4);
  tft.println("Teplota izby");

}

void funk3(){ //TEPLOTA IZBY
//	tft.fillRect(200, 265, 90, 30, BLUE);

  sprintf(string, "Aktualna  %s C", String(msg.v_1.v).c_str());
  tft.setCursor(10,265);
  tft.setTextColor(BLACK); tft.setTextSize(3);
  tft.println(string);

//	tft.fillRect(200, 292, 90, 30, BLUE);
  sprintf(string,"Nastavena %s C", String(msg.v_2.v).c_str());
tft.setCursor(10,292);
  tft.setTextColor(MAGENTA); tft.setTextSize(3);
  tft.println(string);

/*tft.setCursor(330,270);
  tft.setTextColor(BLUE); tft.setTextSize(4);
  tft.println("Chladi");*/
tft.setCursor(350,270);
  tft.setTextColor(RED); tft.setTextSize(4);
  tft.println("Kuri");
}


// Světelný senzor TEMT6000


// vytvoření proměnných pro výsledky měření
float analogHodnota;
int prepocet;

void setup() {

  //nastavenie datovej struktury
  msg.stx = STX;//povinna konstanta
	msg.etx = ETX;//povinna konstanta
  msg.v_1.v = 20.0;//aktualna teplota
  msg.v_2.v = 10.0;// prednastaneva teplota
  msg.v_3.v = 0.0;// detektor svetla
  //nastavenie koncovych poloh garaze
  msg.b_1.v.b_00 = true; //kontakt otvorene
  msg.b_1.v.b_01 = false; //kontakt zatvorene
  // koncove polohy zaluzie
  msg.b_1.v.b_02 = true; //kontakt otvorene
  msg.b_1.v.b_03 = false; //kontakt zatvorene
  // kurenie
  msg.b_2.v.b_00 = false; //kurenie zapnute
  msg.b_2.v.b_01 = false; //chladenie zapnute
  msg.b_2.v.b_02 = false; //ochrana zapnuta
  // relatka
  msg.b_2.v.b_00 = true;  //rele_0
  msg.b_2.v.b_01 = true;  //rele_1
  msg.b_2.v.b_02 = false; //rele_2
  msg.b_2.v.b_03 = false; //rele_3
  msg.b_2.v.b_04 = true;  //rele_4
  msg.b_2.v.b_05 = false; //rele_5
	// LED
	msg.b_3.v.b_00 = false;

  //nastvanie vstupov
  pinMode(INPUT_PIN_16, INPUT);
  pinMode(INPUT_PIN_17, INPUT);
	// nastavenie LED

pinMode(LED_18, OUTPUT);
// nastavenie vystupov , relatok
  pinMode(RELE0_PIN_23, OUTPUT);
  pinMode(RELE1_PIN_25, OUTPUT);
  pinMode(RELE2_PIN_27, OUTPUT);
  pinMode(RELE3_PIN_29, OUTPUT);
  pinMode(RELE4_PIN_31, OUTPUT);
  pinMode(RELE5_PIN_33, OUTPUT);

  // zahájení komunikace po sériové lince
  // rychlostí 115200 baud
  Serial.begin(115200);

  //nastavi displej
  setup_lcd();
  kresli_blok_1();
	funk1();
  kresli_blok_2();
	funk2();
	kresli_blok_3();
	funk3();

}

void  nacitaj_data(){
    // načtení hodnoty z analogového pinu
  analogHodnota = analogRead(analogPin);

  //nacitaj stav tlacitok
  //nastavenie koncovych poloh garaze
  if( digitalRead(INPUT_PIN_16) == HIGH ){
    msg.b_1.v.b_00 = true; //kontakt otvorene true
  }else{
    msg.b_1.v.b_00 = false; //kontakt otvorene false
  }
  if( digitalRead(INPUT_PIN_17) == HIGH ){
    msg.b_1.v.b_01 = true; //kontakt zatvorene true
  }else{
    msg.b_1.v.b_01 = false; //kontakt zatvorene false
  }
}

void  odoslat_data(){
	if(msg.b_3.v.b_00){
		digitalWrite(LED_18, HIGH);   //LED zapnute
	}else{
		digitalWrite(LED_18, LOW);   //LED zapnute
	}
  if(!msg.b_2.v.b_00){
    digitalWrite(RELE0_PIN_23, HIGH);   //relé0 rozopnute
  }else{
    digitalWrite(RELE0_PIN_23, LOW);    //relé0 zapnute
  }

  if(!msg.b_2.v.b_01){
    digitalWrite(RELE1_PIN_25, HIGH);   //relé1 rozopnute
  }else{
    digitalWrite(RELE1_PIN_25, LOW);    //relé1 zapnute
  }

  if(!msg.b_2.v.b_02){
    digitalWrite(RELE2_PIN_27, HIGH);   //relé1 rozopnute
  }else{
    digitalWrite(RELE2_PIN_27, LOW);    //relé1 zapnute
  }

  if(!msg.b_2.v.b_03){
    digitalWrite(RELE3_PIN_29, HIGH);   //relé1 rozopnute
  }else{
    digitalWrite(RELE3_PIN_29, LOW);    //relé1 zapnute
  }

  if(!msg.b_2.v.b_04){
    digitalWrite(RELE4_PIN_31, HIGH);   //relé1 rozopnute
  }else{
    digitalWrite(RELE4_PIN_31, LOW);    //relé1 zapnute
  }

  if(!msg.b_2.v.b_05){
    digitalWrite(RELE5_PIN_33, HIGH);   //relé5 rozopnute
  }else{
    digitalWrite(RELE5_PIN_33, LOW);    //relé5 zapnute
  }
}

	void  urobit_prepocty(){
	   // přepočet analogové hodnoty z celého rozsahu
	  //  0-1023 na procentuální rozsah 0-100
	  prepocet = map(analogHodnota, 0, 1023, 0, 100);

	  msg.v_3.v = analogHodnota / 1023 * 100;
		// zapnutie LED
		if(msg.v_3.v < 20.0){
			msg.b_3.v.b_00 = true;
		}else{
			msg.b_3.v.b_00 = false;
		}
}

void loop(){
  //zapametaj predosly stav periferii
  TELEGRAM last_msg;
  memcpy(&last_msg, &msg, sizeof(msg));


// nacita data z periferii do struktury data
  nacitaj_data();

// vyhodnot data, urob prepocty
  urobit_prepocty();

// zobrazit data na displej

//vyhodnotenie jasu
  if(last_msg.v_3.v != msg.v_3.v){
    funk1();
  }
	//vyhodnotenie garazovych dveri
  if((last_msg.b_1.v.b_00 != msg.b_1.v.b_00) || (last_msg.b_1.v.b_01 != msg.b_1.v.b_01)){
    funk2();
  }
	//vyhodnotenie teploty v izbe
  if((last_msg.v_1.v != msg.v_1.v) || (last_msg.v_2.v != msg.v_2.v)){
    funk3();
  }

// odoslat data do periferii
  odoslat_data();
  //odoslat data do servera
  encodeTelegram(buffer, &msg, sizeof(msg));
  Serial.println(buffer);

  // pauza
delay(1000);
}
