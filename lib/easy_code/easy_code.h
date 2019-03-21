#ifndef __EASY_CODE_HANDLER_H__
#define __EASY_CODE_HANDLER_H__

#include <Arduino.h>

// celkova bajtova velkost telegramu je: 2*1 + 10*2 + 2*1= 24 bytov
typedef struct {
  byte stx; // allways STX
  byte len; // reserved
  //-------------------------------
  // payload
  //-------------------------------
  word w[10];
  //-------------------------------
  // end of payload
  //-------------------------------
  byte crc; // reserved
  byte etx; // allways ETX
} TELEGRAM;

class Telegram {
public:
  static const byte MSG_LEN = sizeof(TELEGRAM);
  static const byte BUF_LEN = MSG_LEN * 2 + 1;
  // temp char
  static const byte ADD = 0x41;
  // start of telegram
  static const byte STX = 0x02;
  // end of telegram
  static const byte ETX = 0x03;
  // offset of first byte in telegram
  static const byte START = 0;
  // offset of last byte in telegram
  static const byte STOP = MSG_LEN - 1;
  // datove objekty
  TELEGRAM msg;
  byte buffer[Telegram::BUF_LEN];
  // metody
  Telegram();
  // urobi vzpis pola telegram
  void logTelegram(){};
  // urobi vzpis pola Buffer
  void logBuffer(){};
  // vrati pole telegram
  byte *getTelegram() { return (byte *)&this->msg; };
  // vrati pole Buffer
  byte *getBuffer() { return this->buffer; };
  // prekonvertuje string typu ASCII do pola Buffer
  void setBuffer(char *str);
  // prekonvertuje string typu ASCII do pola Telegram
  void setTelegram(char *str);
  // nastavi Uint8 hodnotu v poli TELEGRAM
  void setByteInTelegram(int num, byte val);
  // vrati Uint8 hodnotu v poli Telegram
  byte getByteInTelegram(int num);
  // nastavi Uint8 hodnotu v poli buffer
  void setByteInBuffer(int num, byte val);
  // vrati Uint8 hodnotu v poli Buffer
  byte getByteInBuffer(int num);
  // vrati Uint16 hodnotu z payload pola
  // num index v poli
  word getUint16(int num);
  // nastavi Uint16 hodnotu z payload pola
  // num index v poli
  void setUint16(int num, word val);
  // prekontroluje ci je telegram validny
  // vrati true ak je validny, false ak nevalidny
  bool isValidTelegram();
  // prekonvertuje telegram do char retazca
  void encodeTelegram();
  // prekonvertuje char retazec ukonceny '\x00' na telegram
  void decodeTelegram();
  // prekonvertuje cislo word do float
  union Cislo {
    byte sig : 1;  // znamienko
    byte cele : 8; // cela cast cisla
    byte des : 7;  // desatinna cast cisla
    word w;
  };

  float cnvWtoF(const word w_val) {
    union Cislo num;
    float f_val;
    num.w = w_val;
    f_val = 1.0 * num.des / 100.0;
    f_val += num.cele;

    if (num.sig)
      f_val *= -1;
    return f_val;
  };

  // prekonvertuje cislo float do word
  //			-32768 32767
  word cnvFtoW(const float f_val) {
    union Cislo num;
    float f = round(f_val * 100.0) / 100.0;
    int i = f_val;
    if (i < 0) {
      num.sig = 1;
    } else {
      num.sig = 0;
    }
    i = (f - i) * 100.0;
    num.des = i;
    return num.w;
  };
};

#endif
