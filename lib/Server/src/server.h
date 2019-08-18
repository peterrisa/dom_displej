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

#include <Arduino.h>

//komunikacia so serverom
#include <telegram.h>

//server
class Server
{
public:
    enum MASKS
    { // konstanty pre kodovanie povelu pre Arduino
        //ziadny povel
        CMD_NONE = 0x0000,
        // povel garaz otvorit
        CMD_GAR_ON = 0x0001,
        // povel garaz zatvorit
        CMD_GAR_OFF = 0x0002,
        // povel garaz zatvorit
        CMD_GAR_STOP = 0x0004,
        // povel svetlo zapnut
        CMD_LIT_ON = 0x0008,
        // povel svetlo vypnut
        CMD_LIT_OFF = 0x0010,
        // povel svetlo automat
        CMD_LIT_AUT = 0x0020,
        // povel nastavenie teploty
        CMD_TEM_SET = 0x0040,
  // konstanty pre kodovanie stavov
        //ziadny stav
        STA_NONE = 0x0000,
        // chcem aby sa garaz otvorila
        STA_GAR_ON = 0x0001,
        // chcem aby sa garaz zatvorila
        STA_GAR_OFF = 0x0002,
        // chcem aby sa garaz zastavila
        STA_GAR_STOP = 0x0004,
        // stav chcem mat zapnute svetlo
        STA_LIT_ON = 0x0008,
        // stav chce mat vypnute svetlo
        STA_LIT_OFF = 0x0010,
        // stav chcem mat automaticke svetlo
        STA_LIT_AUT = 0x0020,
     // konstanty pre kodovanie feedbacku od arduina
        // stav garaz otvorena
        FBK_GAR_ON = 0x0001,
        // stav garaz zatvorena
        FBK_GAR_OFF = 0x0002,
        // stav garaz zastavena
        FBK_GAR_STOP = 0x0004,
        // stav zapnutie svetla
        FBK_LIT_ON = 0x0008,
        // stav vypnutie svetla
        FBK_LIT_OFF = 0x0010,
        // stav svetlo v automate
        FBK_LIT_AUT = 0x0020,
        // stav kurenie zapnute
        FBK_HEAT_ON = 0x0080,
        // stav chladenie zapnute
        FBK_COOL_ON = 0x0100,
        // stav ochrana zapnuta
        FBK_SAFE_ON = 0x0200,
        // stav rele_0
        FBK_RELE_0 = 0x0400,
        // stav rele_1
        FBK_RELE_1 = 0x0800,
        // stav rele_2
        FBK_RELE_2 = 0x1000,
        // stav rele_3
        FBK_RELE_3 = 0x2000,
        // stav rele_4
        FBK_RELE_4 = 0x4000,
        // stav rele_5
        FBK_RELE_5 = 0x8000
    };
    enum WORDS
    {
        // word setpoint teplota
        STM = 0,
        // word actual teplota
        ATM = 1,
        // word actual osvitu
        ATB = 2,
        // status word
        STA = 7,
        // feedback word
        FDB = 8,
        // command word
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
    // odosli data do serveru
    void doWriteMessage();
    // nacitaj data zo serveru
    void doReadMessage();

private:
    // komunikacia
    Telegram telegram;
};
