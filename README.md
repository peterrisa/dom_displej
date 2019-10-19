# Smart HOUM displej


Kód k displeju s obnovovaním premenných hodnôt. Projekt je súčasťou projektu "Smart HOUM". Projekt je vytvorený v multifunkčnom editore [PlatformIO](https://platformio.org). Vyvýjaný v prostredí [VSCode](https://visualstudio.com), aj napriek výhradám voči politike firmy Microsoft.

## Funkcie
Displej zobrazuje vyhodnotené hodnoty zo senzorov a stav garáže. Hodnotami sa rozumie okolitá teplota, okolitý jas.

## Quick start

Na spustenie projektu je nutné mať k dispozícii Arduino (ideálne Mega 2560). Taktiež je potrebné mať nejaký editor, pomocou ktorého sa nahrá kód do Arduina. Odporúčam základné Arduino IDe, alebo PlatformIO.

## Schéma zapojenia:

Displej:
-------------POWER Pins--------------------------------------
   5V  connects to DC 5V
   GND connects to Ground
   3V3 do not need to connect(NC)
--------------LCD Display Pins--------------------------------
   LCD_RD   connects to Analog pin A0
   LCD_WR   connects to Analog pin A1
   LCD_RS   connects to Analog pin A2
   LCD_CS   connects to Analog pin A3
   LCD_RST  connects to Analog pin A4
   LCD_D0   connects to digital pin 8
   LCD_D1   connects to digital pin 9
   LCD_D2   connects to digital pin 2
   LCD_D3   connects to digital pin 3
   LCD_D4   connects to digital pin 4
   LCD_D5   connects to digital pin 5
   LCD_D6   connects to digital pin 6
   LCD_D7   connects to digital pin 7
--------------SD-card fuction Pins ---------------------------
 This Connection Only for UNO, Do not support Mega2560
 because they use different Hardware-SPI Pins
 SD_SS    connects to digital pin 10
 SD_DI    connects to digital pin 11
 SD_DO    connects to digital pin 12
 SD_SCK   connects to digital pin 13
--------------------------------------------------------------

Relé:


























Up>
Up>

