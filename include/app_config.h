#ifndef __APP_CONFIG_HANDLER_H__
#define __APP_CONFIG_HANDLER_H__

//nastavenie pinov pre motory
//motor garaze A
#define PWM_A 46
#define IN1_A 42
#define IN2_A 40
//motor zaclony B
#define PWM_B 44
#define IN1_B 38
#define IN2_B 36
// nastavení čísla prepojovanieho pinu
#define analogPin A6
// konstanty pre teplomery
#define MAX_DS1820_SENSORS 5
#define INPUT_PIN_14 14
// kontakty pre vstupy
#define INPUT_PIN_15 15
#define INPUT_PIN_16 16
// konstanty pre relatka

#define RELE0 53    //rezerva
#define RELE1 51    //rezerva
#define RELE2 49    //rezerva
#define RELE3 47    //svetlo
#define RELE4 45    //pohyb smer garaze
#define RELE5 43    //motor garaze
// konstanty pre displej
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3    // Chip Select goes to Analog 3
#define LCD_CD A2    // Command/Data goes to Analog 2
#define LCD_WR A1    // LCD Write goes to Analog 1
#define LCD_RD A0    // LCD Read goes to Analog 0
//--------------------------------------------------------------
// Farby
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GREY 0x7BCF
#define BLUGREEN 0x0B74
#define WHITE 0xFFFF

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a, b) ((a) &= ~(1ULL << (b)))
#define BIT_FLIP(a, b) ((a) ^= (1ULL << (b)))
// '!!' to make sure this returns 0 or 1
#define BIT_CHECK(a, b) (!!((a) & (1ULL << (b))))

/* x=target variable, y=mask */
#define BITMASK_SET(x, y) ((x) |= (y))
#define BITMASK_CLEAR(x, y) ((x) &= (~(y)))
#define BITMASK_FLIP(x, y) ((x) ^= (y))
// warning: evaluates y twice
#define BITMASK_CHECK_ALL(x, y) (((x) & (y)) == (y))
#define BITMASK_CHECK_ANY(x, y) ((x) & (y))

// konstanty pre kodovanie povelu pre Arduino
// povel garaz otvorit
#define CMD_GAR_ON 0x0001
// povel garaz zatvorit
#define CMD_GAR_OFF 0x0002
// povel garaz zatvorit
#define CMD_GAR_STOP 0x0004
// povel svetlo zapnut
#define CMD_LIT_ON 0x0008
// povel svetlo vypnut
#define CMD_LIT_OFF 0x0010
// povel svetlo automat
#define CMD_LIT_AUT 0x0020
// povel nastavenie teploty
#define CMD_TEM_SET 0x0040
// konstanty pre kodovanie stavov
// chcem aby sa garaz otvorila
#define STA_GAR_ON 0x0001
// chcem aby sa garaz zatvorila
#define STA_GAR_OFF 0x0002
// chcem aby sa garaz zastavila
#define STA_GAR_STOP 0x0004
// stav chcem mat zapnute svetlo
#define STA_LIT_ON 0x0008
// stav chce mat vypnute svetlo
#define STA_LIT_OFF 0x0010
// stav chcem mat automaticke svetlo
#define STA_LIT_AUT 0x0020
// konstanty pre kodovanie feedbacku od arduina
// stav garaz otvorena
#define FBK_GAR_ON 0x0001
// stav garaz zatvorena
#define FBK_GAR_OFF 0x0002
// stav garaz zastavena
#define FBK_GAR_STOP 0x0004
// stav zapnutie svetla
#define FBK_LIT_ON 0x0008
// stav vypnutie svetla
#define FBK_LIT_OFF 0x0010
// stav svetlo v automate
#define FBK_LIT_AUT 0x0020
// stav kurenie zapnute
#define FBK_HEAT_ON 0x0080
// stav chladenie zapnute
#define FBK_COOL_ON 0x0100
// stav ochrana zapnuta
#define FBK_SAFE_ON 0x0200
// stav rele_0
#define FBK_RELE_0 0x0400
// stav rele_1
#define FBK_RELE_1 0x0800
// stav rele_2
#define FBK_RELE_2 0x1000
// stav rele_3
#define FBK_RELE_3 0x2000
// stav rele_4
#define FBK_RELE_4 0x4000
// stav rele_5
#define FBK_RELE_5 0x8000

// word setpoint teplota
#define STM 0
// word actual teplota
#define ATM 1
// word actual osvitu
#define ATB 2
// status word
#define STA 7
// feedback word
#define FDB 8
// command word
#define CMD 9

#endif
