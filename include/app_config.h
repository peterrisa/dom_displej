#ifndef __APP_CONFIG_HANDLER_H__
#define __APP_CONFIG_HANDLER_H__

// nastavení čísla prepojovanieho pinu
#define analogPin A6
//kontakty pre vstupy
#define INPUT_PIN_16 16
#define INPUT_PIN_17 17

//konstanty pre relatka
#define RELE0_PIN_23 23
#define RELE1_PIN_25 25
#define RELE2_PIN_27 27
#define RELE3_PIN_29 29
#define RELE4_PIN_31 31
#define RELE5_PIN_33 33

//konstanty pre displej

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3   // Chip Select goes to Analog 3
#define LCD_CD A2  // Command/Data goes to Analog 2
#define LCD_WR A1  // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

//konstanty pre koncoveé spínače
#define pinD 14
#define pinD 15
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

#endif
