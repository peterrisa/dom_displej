#pragma once

#include <Arduino.h>

//teplotné čidlá cez onewire
#include <OneWire.h>
#include <DallasTemperature.h>

//ovládanie motorov
#include <motolib.h>

//žalúzie
//motor záclony B
const int PWM_B_PIN = 44;
const int IN1_B_PIN = 38;
const int IN2_B_PIN = 36;
const int ZACLONY_SPEED = 200;
const int ZACLONY_DELAY_TO_TRANSFER = 1000;

class Louver
{
public:
    //setup louver(záclony)
    void setup(int en, int in1, int in2);
    //ovládanie žalúzií - odtiahnuť
    void doLouverOpen();
    //ovládanie žalúzií - zatiahnuť
    void doLouverClose();
    //vrať či je žalúzia otvorená
    bool isOpened() { return louverPosition; }

private:
    // pomocná premenná pre definovanie polohy záclon |true = otvorené|, |false = zatvorené|
    bool louverPosition = false;
    // motor žalúziǐ
    LM298N_bridge motor;

public:
    // rýchlosť otvárania
    unsigned char MaxSpeed = 50;
    // oneskorenie prenosu (delay transfer)
    int DelayTransfer = 1000;
};

//---Garíž---

// kontakty pre vstupy
const int OPEN_LIMIT_PIN = 15;
const int CLOSE_LIMIT_PIN = 16;

//nastavenie pinov pre motory
//motor garáže A
const int PWM_A_PIN = 46;
const int IN1_A_PIN = 42;
const int IN2_A_PIN = 40;
//rýchlosť zatvárania garáže
const int GARAGE_SPEED_CLOSE = 225;
//rýchlosť otvárania garáže
const int GARAGE_SPEED_OPEN = 255;

class Garage
{
public:
    Garage() { _isOpening = _isClosing = _openLimit = _closeLimit = false; }
    //setup garáže
    void setup(int en, int in1, int in2);
    //zastav garáž
    void doStop();
    //otvor garáž
    void doOpen();
    //zatvor garáž
    void doClose();
    //skontroluj, či sa otvára
    bool isOpening() { return _isOpening; }
    //vrať či sa zatvra
    bool isClosing() { return _isClosing; }
    //skontroluj či je otvorený
    bool isOpened() { return _openLimit; }
    //vrať či je zatvorený
    bool isClosed() { return _closeLimit; }
    //nastav limit |open|
    void setOpened(bool);
    //nastav limit |close|
    void setClosed(bool);

private:
    // motor garážových dverí
    LM298N_bridge motor;
    //otvára sa, motor bezi
    bool _isOpening;
    //zatvýra sa, motor bezi
    bool _isClosing;
    //je otvorený, dosiahol koncovú polohu
    bool _openLimit;
    //je zatvorený, dosiahol koncovú polohu
    bool _closeLimit;

public:
    //speed to open
    unsigned char MaxSpeedToOpen = 50;
    // speed to close
    unsigned char MaxSpeedToClose = 50;
};

//---Svetlo---
//automatické zapínania od stavu okolitého jasu
const int ZAPNUTIE_SVETIEL_HRANICA = 20;
const int ZATVORENIE_ZACLON_HRANICA = 50;
const int LIGHT_SENSOR_PIN = A6;
const int LIGHT_RELAY_PIN = 53; //51
// konštanty pre relátka

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
    //zisti okolitý jas
    int doLightReadExposure();
    //aky je okolitý jas
    int doLightGetExposure() { return _osvit; }
    bool isLightOn() { return _isLightOn; }
    bool isAutomat() { return _isAutomat; }

private:
    // svetlo |zapnute > true|, |vypnute > false|
    bool _isLightOn;
    // automat je |zapnuty > true|, |vypnuty > false|
    bool _isAutomat;
    //osvit nacitany z čidla
    int _osvit;
};

//teplomer
// maximálny počet teplomerov DS1820 na pine
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
    // vytvoeenie instance oneWireDS z knihovny OneWire
    OneWire _oneWireDS; // teplomery sú na pine 14
    // vytvořenie instance senzoryDS z knihovny DallasTemperature
    DallasTemperature _senzoryDS;
    int _num_temp = 0; // počet teplomerov pripojených
    byte _addr[MAX_DS1820_SENSORS][8];
    char _buf[20];
    // koľko môže byť DS18S20 teplomerov pripojených
    // pole teplôt
    // teplota[0] = T1 - teplota okolia
    float _teplota[MAX_DS1820_SENSORS];
};
