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

#include <Arduino.h>

//teplotne cidla cez onewire
#include <OneWire.h>
#include <DallasTemperature.h>

//ovladanie motorov
#include <motolib.h>

//komunikacia so serverom
#include <telegram.h>

//zaluzie
//motor zaclony B
const int PWM_B_PIN = 44;
const int IN1_B_PIN = 38;
const int IN2_B_PIN = 36;
const int ZACLONY_SPEED = 200;
const int ZACLONY_DELAY_TO_TRANSFER = 1000;

class Louver
{
public:
    //setup louver
    void setup(int en, int in1, int in2);
    //ovladanie zaluzii odtiahnut
    void doLouverOpen();
    //ovladanie zaluzii zatiahnut
    void doLouverClose();

private:
    // pomocná premenná pre definovanie polohy záclom true = otvorené, false = zatvorené
    bool louverPosition = false;
    // motor zaluzii
    LM298N_bridge motor;

public:
    // speed to open
    unsigned char MaxSpeed = 50;
    // delay transfer
    int DelayTransfer = 1000;
};

// garaz

// kontakty pre vstupy
const int OPEN_LIMIT_PIN = 15;
const int CLOSE_LIMIT_PIN = 16;

//nastavenie pinov pre motory
//motor garaze A
const int PWM_A_PIN = 46;
const int IN1_A_PIN = 42;
const int IN2_A_PIN = 40;
const int GARAGE_SPEED_CLOSE = 225;
const int GARAGE_SPEED_OPEN = 255;

class Garage
{
public:
    //setup garage
    void setup(int en, int in1, int in2);
    //zastav garaz
    void doStop();
    //otvor garaz
    void doOpen();
    //zatvor garaz
    void doClose();
    //vrat ci sa otvara
    bool isOpening() { return _isOpening; };
    //vrat ci sa zatvara
    bool isClosing() { return _isClosing; };
    //nastav limit open
    void setOpened(bool);
    //nastav limit close
    void setClosed(bool);

private:
    // motor garazovych dveri
    LM298N_bridge motor;
    //otvara sa, motor bezi
    bool _isOpening;
    //zatvara sa, motor bezi
    bool _isClosing;
    // je otvoreny, ma koncak
    bool _openLimit;
    // je zatvoreny, ma koncak
    bool _closeLimit;

public:
    // speed to open
    unsigned char MaxSpeedToOpen = 50;
    // speed to close
    unsigned char MaxSpeedToClose = 50;
};

//svetlo
//automaticke zapinania od osvitu
const int ZAPNUTIE_SVETIEL_HRANICA = 20;
const int ZATVORENIE_ZACLON_HRANICA = 50;
const int LIGHT_SENSOR_PIN = A6;
const int LIGHT_RELAY_PIN = 51;
// konstanty pre relatka

class Light
{
public:
    void setup();
    //zapne svetlo
    void doLightTurnOn();
    //vypne svetlo
    void doLightTurnOff();
    //zapne automat
    void doLightStartAutomat();
    //vypne automat
    void doLightStopAutomat();
    //zisti osvit
    int doLightReadExposure();
    //aky je osvit
    int doLightGetExposure()
    {
        return _osvit;
    };

private:
    // svetlo zapnute > true, vypnute > false
    bool _isLightOn = false;
    // automat je zapnuty > true, vypnuty > false
    bool _isAutomat = false;
    //osvit nacitany z cidla
    int _osvit = 0;
};

//server
class Server
{
public:
    enum CMD
    { // konstanty pre kodovanie povelu pre Arduino
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
        CMD_TEM_SET = 0x0040
    };
    enum STA
    { // konstanty pre kodovanie stavov
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
        STA_LIT_AUT = 0x0020
    };
    enum FBK
    { // konstanty pre kodovanie feedbacku od arduina
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
    enum Words
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
    // odosli data do serveru
    void doWriteMessage();
    // nacitaj data zo serveru
    void doReadMessage();

public:
    // komunikacia
    Telegram telegram;
};

//teplomer
// maximalny pocet teplomerov DS1820 na pine
const int MAX_DS1820_SENSORS = 5;
//one wire pin
const uint8_t TEMP_INPUT_PIN = 14;

class Temp
{
public:
    Temp();
    void doRead();
    float getTemp() { return _teplota[0]; }

private:
    // vytvoření instance oneWireDS z knihovny OneWire
    OneWire _oneWireDS; // teplomery sú na pine 14
    // vytvoření instance senzoryDS z knihovny DallasTemperature
    DallasTemperature _senzoryDS;
    int _num_temp = 0; // pocet teplomerov pripojenych
    byte _addr[MAX_DS1820_SENSORS][8];
    char _buf[20];
    // koľko môže byť DS18S20 teplomerov pripojených
    // pole teplôt
    // teplota[0] = T1 - teplota okolia
    float _teplota[MAX_DS1820_SENSORS];
};
