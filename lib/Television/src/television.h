#pragma once

#include <Arduino.h>

//riadenie displeja
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include <MCUFRIEND_kbv.h>
//televizor

// konstanty pre displej
const int LCD_RESET = A4; // Can alternately just connect to Arduino's reset pin
const int LCD_CS = A3;    // Chip Select goes to Analog 3
const int LCD_CD = A2;    // Command/Data goes to Analog 2
const int LCD_WR = A1;    // LCD Write goes to Analog 1
const int LCD_RD = A0;    // LCD Read goes to Analog 0
//--------------------------------------------------------------
// Farby
const int BLACK = 0x0000;
const int BLUE = 0x001F;
const int RED = 0xF800;
const int GREEN = 0x07E0;
const int CYAN = 0x07FF;
const int MAGENTA = 0xF81F;
const int YELLOW = 0xFFE0;
const int GREY = 0x7BCF;
const int BLUGREEN = 0x0B74;
const int WHITE = 0xFFFF;

class Television
{
public:
    Television();
    void setup();
    //svetla
    void setLight(bool);
    //nastav teplotu
    void setCurTemp(int temp);
    void setSetTemp(int temp);
    //nastav osvit
    void setExposition(int);
    //nastav limit open
    void setOpened(bool);
    //nastav limit close
    void setClosed(bool);
    //kresli celu obrazovku
    void drawScreen();
    //kresli pozadie blokov
    void doDrawBlock1();
    void doDrawBlock2();
    void doDrawBlock3();
    //kresli vnutro blokov
    void doRedrawBlock1();
    void doRedrawBlock2();
    void doRedrawBlock3();

private:
    //teplota
    int _setTemp;
    int _curTemp;
    //osvit
    bool _light;
    int _osvit;
    // je otvoreny, ma koncak
    bool _openLimit;
    // je zatvoreny, ma koncak
    bool _closeLimit;
    //poziadavka na prekreslenie casti obrazovky
    bool _drawBlock1;   //nakresli pozadie za blokom1
    bool _redrawBlock1; //prekresli blok1 aj je true
    bool _drawBlock2;   //nakresli pozadie za blokom1
    bool _redrawBlock2; //prekresli _redrawBlock2 aj je true
    bool _drawBlock3;   //nakresli pozadie za blokom1
    bool _redrawBlock3; //prekresli blok3 aj je true
    //--------------------------------------------------------------
    // nastavenia pre displej
    //--------------------------------------------------------------
    char _string[256];
    MCUFRIEND_kbv _tft;
};