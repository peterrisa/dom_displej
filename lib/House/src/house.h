#pragma once

#include <Arduino.h>

//teplotne cidla cez onewire
#include <OneWire.h>
#include <DallasTemperature.h>

//ovladanie motorov
#include <motolib.h>

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
    //vrat ci je zaluzia otvorena
    bool isOpened() { return louverPosition; }

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
    Garage() { _isOpening = _isClosing = _openLimit = _closeLimit = false; }
    //setup garage
    void setup(int en, int in1, int in2);
    //zastav garaz
    void doStop();
    //otvor garaz
    void doOpen();
    //zatvor garaz
    void doClose();
    //vrat ci sa otvara
    bool isOpening() { return _isOpening; }
    //vrat ci sa zatvara
    bool isClosing() { return _isClosing; }
    //vrat ci je otvoreny
    bool isOpened() { return _openLimit; }
    //vrat ci je zatvoreny
    bool isClosed() { return _closeLimit; }
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
const int LIGHT_RELAY_PIN = 53; //51
// konstanty pre relatka

class Light
{
public:
    Light()
    {
        _isLightOn = _isAutomat = false;
        _osvit = 0;
    };
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
    int doLightGetExposure() { return _osvit; }
    bool isLightOn() { return _isLightOn; }
    bool isAutomat() { return _isAutomat; }

private:
    // svetlo zapnute > true, vypnute > false
    bool _isLightOn;
    // automat je zapnuty > true, vypnuty > false
    bool _isAutomat;
    //osvit nacitany z cidla
    int _osvit;
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

    //nastav teplotu
    void setSetTemp(int temp){_setTemp = temp;}
    int getSetTemp(){return _setTemp;}

private:
    //teplota
    int _setTemp;
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
