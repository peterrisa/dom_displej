#include "easy_code.h"
#include <Arduino.h>

//-----------------------------------------------------------
// prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool isValidTelegram(const TELEGRAM *msg) {
  if (msg->stx != STX)
    return false;
  if (msg->etx != ETX)
    return false;
  return true;
}

// prekonvertuje telegram do char retazca ukonceneho '\x00'
// buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
// buffer - pole znakov kam sa bude konvertovat
// msg - struktura telegramu
// len - velkost struktury telegramu v bajtoch; sizeof(TELEGRAM)
void encodeTelegram(byte *buffer, const TELEGRAM *msg, const int len) {
  byte *b_msg = (byte *)msg;
  byte *buf = buffer;
  byte a, b;

  for (int i = 0; i < len; i++, b_msg++) {
    a = *b_msg & 0x0F;
    b = (*b_msg & 0xF0) >> 4;
    *buf++ = a + ADD;
    *buf++ = b + ADD;
  }
  *buf = '\x00';
}

// prekonvertuje char retazec ukonceny '\x00' na telegram
// buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
// buffer - pole znakov odkial sa bude konvertovat
// msg - struktura telegramu
// len - velkost struktury telegramu v bajtoch; int len = sizeof(TELEGRAM);
void decodeTelegram(TELEGRAM *msg, const int len, const byte *buffer) {
  byte *b_msg = (byte *)msg;
  byte *buf = buffer;
  byte a, b;

  for (int i = 0; i < len; i++, b_msg++) {
    a = *buf++ - ADD;
    b = *buf++ - ADD;
    *b_msg = (a & 0x0F) | ((b & 0x0F) << 4);
  }
}

void test() {
  Serial.print("velkost F_VALUE ma byt 4: ");
  Serial.println(sizeof(F_VALUE));
  Serial.print("velkost B_VALUE ma byt 2: ");
  Serial.println(sizeof(B_VALUE));
  Serial.print("velkost TELEGRAM ma byt 26: ");
  Serial.println(sizeof(TELEGRAM));

  TELEGRAM msg_orig;
  msg_orig.stx = STX;
  msg_orig.etx = ETX;
  msg_orig.v_1.v = -99.99;
  msg_orig.v_2.v = 99.99;
  msg_orig.v_3.v = -25.25;
  msg_orig.v_4.v = 25.25;
  msg_orig.b_1.b[0] = 0xAA;
  msg_orig.b_1.b[1] = 0x55;
  msg_orig.b_2.b[0] = 0xAA;
  msg_orig.b_2.b[1] = 0x55;
  msg_orig.b_3.b[0] = 0xAA;
  msg_orig.b_3.b[1] = 0x55;
  msg_orig.b_4.b[0] = 0xAA;
  msg_orig.b_4.b[1] = 0x55;

  char buffer[2 * sizeof(TELEGRAM) + 1];
  encodeTelegram(buffer, &msg_orig, sizeof(msg_orig));

  Serial.print("zakodovany telegram: ");
  Serial.println(buffer);
  Serial.print("odkodovany telegram: ");
  Serial.println((char *)&msg_orig);

  TELEGRAM msg_copy;
  decodeTelegram(&msg_copy, sizeof(msg_copy), buffer);
  Serial.print("odkodovany telegram: ");
  Serial.println((char *)&msg_copy);

  byte *orig = (byte *)&msg_orig;
  byte *copy = (byte *)&msg_copy;

  for (int i = 0; i < sizeof(TELEGRAM); i++) {
    if (orig[i] == copy[i]) {
      Serial.print("   zhoda  : ");
      Serial.println(i);
    } else {
      Serial.print("!! nezhoda: ");
      Serial.print(orig[i]);
      Serial.print(" : ");
      Serial.print(copy[i]);
      Serial.print(" : ");
      Serial.println(i);
    }
  }
  Serial.print("   msg_orig je validny  : ");
  Serial.println(isValidTelegram(&msg_orig));
  Serial.print("   msg_copy je validny  : ");
  Serial.println(isValidTelegram(&msg_copy));
}

void setup() {
  Serial.begin(115200); // opens serial port
  test();
}

void loop() {}
