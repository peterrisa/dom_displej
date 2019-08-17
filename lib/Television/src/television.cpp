#include <television.h>

Television::Television()
{
    _drawBlock1 = true; //prekresli blok1 ak je true
    _drawBlock2 = true; //prekresli blok2 ak je true
    _drawBlock3 = true; //prekresli blok3 ak je true
}

void Television::setup()
{
    Serial.println(F("TFT LCD test"));
    _tft.reset();

    // Zoznam podporovaných displejov
    uint16_t identifier = _tft.readID();
    if (identifier == 0x9325)
    {
        Serial.println(F("Found ILI9486 LCD driver"));
    }
    else if (identifier == 0x9488)
    {
        Serial.print(F("Unknown LCD driver chip: "));
        Serial.println(identifier, HEX);
        Serial.println(
            F("If using the Adafruit 3.5\" TFT Arduino shield, the line:"));
        Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
        Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
        Serial.println(
            F("If using the breakout board, it should NOT be #defined!"));
        Serial.println(
            F("Also if using the breakout, double-check that all wiring"));
        Serial.println(F("matches the tutorial."));
        identifier = 0x9486;
    }
    _tft.begin(identifier);

    _tft.fillScreen(BLACK);
    //unsigned long start =
    micros();
    // Otočenie obrazovky (o 90 stupňov) (0=0, 1=90, 2=180, 3=270)
    _tft.setRotation(3);
}

//--------------------------------------------------------------
// SVETLA
//--------------------------------------------------------------
void Television::doDrawBlock1()
{ // SVETLA
    _drawBlock1 = false;

    // vykreslenie oblého štvorca
    //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
    _tft.fillRoundRect(3, 0, 230, 210, 10, WHITE);
    // Text (Stvorec 2) - Svetla
    _tft.setCursor(50, 10);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(4);
    _tft.println("Svetla");

    _tft.setCursor(68, 65);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(3);
    _tft.println("Spalna");

    _tft.setCursor(45, 95);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(3);
    _tft.println("Obyvacka");

    _tft.setCursor(75, 125);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(3);
    _tft.println("Garaz");
    //nakresli vnutro bloku
    _redrawBlock1 = true;
}
//--------------------------------------------------------------
// kresli jas
//--------------------------------------------------------------
void Television::doRedrawBlock1()
{ // SVETLA
    _redrawBlock1 = false;

    // prekresli pozadie textu
    _tft.fillRect(45, 165, 170, 30, WHITE);
    // nastav parametre textu
    _tft.setCursor(45, 170);
    _tft.setTextColor(GREY);
    _tft.setTextSize(3);
    // zobraz text s hodnotou osvitu
    sprintf(_string, "Jas %s", String(_osvit).c_str());
    _tft.println(_string);
}
//--------------------------------------------------------------
// nakresli obdlznik s grafikou garaze
//--------------------------------------------------------------
void Television::doDrawBlock2()
{ // GARAZ
    _drawBlock2 = false;
    // vykreslenie oblého štvorca
    //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
    _tft.fillRoundRect(245, 0, 225, 210, 10, WHITE);
    // Text (Stvorec 1) - Garaz
    _tft.setCursor(300, 10);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(4);
    _tft.println("Garaz");
    //nakresli vnutro bloku
    _redrawBlock2 = true;
}
//--------------------------------------------------------------
// kresli garaze
//--------------------------------------------------------------
void Television::doRedrawBlock2()
{ // GARAZ
    _redrawBlock2 = false;

    _tft.fillRect(280, 55, 160, 30, WHITE);

    _tft.setCursor(280, 55);
    _tft.setTextColor(RED);
    _tft.setTextSize(3);

    // Garaz otvorena obrazok + text
    if (_openLimit)
    {
        _tft.setCursor(290, 55);
        _tft.setTextColor(GREEN);
        _tft.setTextSize(3);
        _tft.println("Otvorena");
        _tft.fillRect(267, 100, 180, 100, BLACK);
        _tft.fillRect(279, 110, 155, 16, WHITE);
    }
    // Garaz zatvorena text + obrazok
    if (_closeLimit)
    {
        _tft.setCursor(280, 55);
        _tft.setTextColor(RED);
        _tft.setTextSize(3);
        _tft.println("Zatvorena");
        _tft.fillRect(267, 100, 180, 100, BLACK);
        _tft.fillRect(279, 110, 155, 16, WHITE);
        _tft.fillRect(279, 131, 155, 16, WHITE);
        _tft.fillRect(279, 152, 155, 16, WHITE);
        _tft.fillRect(279, 173, 155, 16, WHITE);
    }
    // Garaz pracuje
    if (!_openLimit && !_closeLimit)
    {
        _tft.setCursor(290, 55);
        _tft.setTextColor(MAGENTA);
        _tft.setTextSize(3);
        _tft.println("PRACUJEM");
        _tft.fillRect(267, 100, 180, 100, BLACK);
        _tft.fillRect(279, 110, 155, 16, WHITE);
        _tft.fillRect(279, 131, 155, 16, WHITE);
    }
}
//--------------------------------------------------------------
// blok s teplotami
//--------------------------------------------------------------
void Television::doDrawBlock3()
{ // TEPLOTA IZBY
    _drawBlock3 = false;
    // vykreslenie oblého štvorca
    //(osa X, osa Y, rozmery X, Rozmery Y, zaoblenie, farba)
    _tft.fillRoundRect(3, 220, 474, 100, 10, WHITE);

    // Text (Stvorec 3)- Teplota izby
    _tft.setCursor(110, 225);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(4);
    _tft.println("Teplota izby");
    //nakresli vnutro bloku
    _redrawBlock3 = true;
}

void Television::doRedrawBlock3()
{ // TEPLOTA IZBY

    _redrawBlock3 = false;

    _tft.fillRect(185, 265, 80, 50, WHITE);

    sprintf(_string, "Aktualna  %s C", String(_curTemp).c_str());
    _tft.setCursor(10, 265);
    _tft.setTextColor(BLACK);
    _tft.setTextSize(3);
    _tft.println(_string);

    //	_tft.fillRect(200, 292, 90, 30, BLUE);
    sprintf(_string, "Nastavena %s C", String(_setTemp).c_str());
    _tft.setCursor(10, 292);
    _tft.setTextColor(MAGENTA);
    _tft.setTextSize(3);
    _tft.println(_string);

    /*_tft.setCursor(330,270);
    _tft.setTextColor(BLUE); _tft.setTextSize(4);
    _tft.println("Chladi");*/
    _tft.setCursor(292, 275);
    _tft.setTextColor(RED);
    _tft.setTextSize(3.5);
    _tft.println("Prihrieva");
}

void Television::drawScreen()
{
    //kresli pozadie jedotlivych blokov
    if (_drawBlock1)
        doDrawBlock1();
    if (_drawBlock2)
        doDrawBlock2();
    if (_drawBlock3)
        doDrawBlock3();
    //kresli obsah blokov
    if (_redrawBlock1)
        doRedrawBlock1();
    if (_redrawBlock2)
        doRedrawBlock2();
    if (_redrawBlock3)
        doRedrawBlock3();
}

//nastav limit open
void Television::setOpened(bool limit)
{
    if (limit != _openLimit)
    {
        _openLimit = limit;
        _redrawBlock2 = true;
    }
}
//nastav limit close
void Television::setClosed(bool limit)
{
    if (limit != _closeLimit)
    {
        _closeLimit = limit;
        _redrawBlock2 = true;
    }
}
//nastav teplotu
void Television::setCurTemp(int temp)
{
    if (_curTemp != temp)
    {
        _curTemp = temp;
        _redrawBlock3 = true;
    }
}
//nastav teplotu
void Television::setSetTemp(int temp)
{
    if (_setTemp != temp)
    {
        _setTemp = temp;
        _redrawBlock3 = true;
    }
}
//nastav osvit
void Television::setExposition(int osvit)
{
    if (_osvit != osvit)
    {
        _osvit = osvit;
        _redrawBlock1 = true;
    }
}
//Svetla
void Television::setLight(bool light)
{
    if (_light != light)
    {
        _redrawBlock1 = true;
        _light = light;
    }
}