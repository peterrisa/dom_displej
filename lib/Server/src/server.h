#pragma once

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

//typedef struct {
//byte stx; // allways STX
//byte len; // reserved
//-------------------------------
//payload
//-------------------------------
//word w_0; // -- setpoint teplota
//word w_1; // -- skutocna teplota
//word w_2; // -- aktualny osvit
//word w_3; // -- reserved
//word w_4; // -- reserved
//word w_5; // -- reserved
//word w_6; // -- tx setpoint teplota
//word w_7; // -- stav vystupov
//word w_8; // -- feedbacky
//word w_9; // -- prikazy
//-------------------------------
//end of payload
//-------------------------------
//byte crc; // reserved
//byte etx; // allways ETX
//TELEGRAM;

#include <Arduino.h>

//komunikácia zo serverom
#include <telegram.h>

//server
class Server
{
public:
    enum MASKS
    { // konštanty pre kódovanie povelu pre Arduino
        //žiadny povel
        CMD_NONE = 0x0000,
        //povel garáž otvoriť
        CMD_GAR_ON = 0x0001,
        //povel garaž zatvoriť
        CMD_GAR_OFF = 0x0002,
        //povel garaž zatvoriť
        CMD_GAR_STOP = 0x0004,
        //povel svetlo zapnuť
        CMD_LIT_ON = 0x0008,
        //povel svetlo vypnuť
        CMD_LIT_OFF = 0x0010,
        //povel svetlo automat
        CMD_LIT_AUT = 0x0020,
        //povel nastavenie teploty
        CMD_TEM_SET = 0x0040,
        //konštanty pre kódovanie stavov
        //žiadny stav
        STA_NONE = 0x0000,
        //chcem aby sa garáž otvorila
        STA_GAR_ON = 0x0001,
        //chcem aby sa garáž zatvorila
        STA_GAR_OFF = 0x0002,
        //chcem aby sa garáž zastavila
        STA_GAR_STOP = 0x0004,
        //stav chcem mať zapnuté svetlo
        STA_LIT_ON = 0x0008,
        //stav chcem mať vypnuté svetlo
        STA_LIT_OFF = 0x0010,
        //stav chcem mať automatické svetlo
        STA_LIT_AUT = 0x0020,
        //konštanty pre kódovanie feedbacku od arduina
        //stav garáž otvorená
        FBK_GAR_ON = 0x0001,
        //stav garáž zatvorená
        FBK_GAR_OFF = 0x0002,
        //stav garáž zastavená
        FBK_GAR_STOP = 0x0004,
        //stav zapnutia svetla
        FBK_LIT_ON = 0x0008,
        //stav vypnutia svetla
        FBK_LIT_OFF = 0x0010,
        //stav svetlo v automatike
        FBK_LIT_AUT = 0x0020,
        //stav kúrenia zapnuté
        FBK_HEAT_ON = 0x0080,
        //stav chladenie zapnuté
        FBK_COOL_ON = 0x0100,
        //stav ochrana zapnutá
        FBK_SAFE_ON = 0x0200,
        //stav relé_0
        FBK_RELE_0 = 0x0400,
        //stav relé_1
        FBK_RELE_1 = 0x0800,
        //stav relé_2
        FBK_RELE_2 = 0x1000,
        //stav relé_3
        FBK_RELE_3 = 0x2000,
        //stav relé_4
        FBK_RELE_4 = 0x4000,
        //stav relé_5
        FBK_RELE_5 = 0x8000
    };
    enum WORDS
    {
        //word setpoint teplota
        STM = 0,
        //word actual teplota
        ATM = 1,
        //word actual osvitu
        ATB = 2,
        //status word
        STA = 7,
        //feedback word
        FDB = 8,
        //command word
        CMD = 9
    };
    Server();
    //set word
    void setWord(Server::WORDS address, word data);
    //get word
    word getWord(Server::WORDS address);
    //set bit
    void setBit(Server::WORDS address, Server::MASKS mask, bool bit);
    //get bit
    bool getBit(Server::WORDS address,Server::MASKS  mask);
    //odosli data do serveru
    void doWriteMessage();
    //nacitaj data zo serveru
    void doReadMessage();

private:
    //komunikacia
    Telegram telegram;
};
