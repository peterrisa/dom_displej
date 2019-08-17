#include <telegram.h>

//--------------------------------------------------------------
// kniznica TELEGRAM
//--------------------------------------------------------------
Telegram::Telegram() {
  // iniciovat datove objekty
  memset((byte *)this->getTelegram(), 0, Telegram::MSG_LEN);
  memset((byte *)this->getBuffer(), 0, Telegram::BUF_LEN);
  // preset telegram header
  setByteInTelegram(Telegram::START, Telegram::STX);
  // preset telegram footer
  setByteInTelegram(Telegram::STOP, Telegram::ETX);
}
// prekonvertuje ascii string do bufferu
void Telegram::setBuffer(char *str) {
  strncpy((char *)this->getBuffer(), str, Telegram::BUF_LEN);
}
// prekonvertuje ascii string do telegramu
void Telegram::setTelegram(char *str) {
  strncpy((char *)this->getTelegram(), str, Telegram::MSG_LEN);
}
// nastavi Uint8 hodnotu v poli TELEGRAM
void Telegram::setByteInTelegram(int num, byte val) {
  if ((num >= 0) && (num < Telegram::MSG_LEN)) {
    byte *uint8 = (byte *)this->getTelegram();
    uint8[num] = val;
  }
}
// vrati Uint8 hodnotu v poli Telegram
byte Telegram::getByteInTelegram(int num) {
  if ((num >= 0) && (num < Telegram::MSG_LEN)) {
    byte *uint8 = (byte *)this->getTelegram();
    return uint8[num];
  }
  return 0;
}
// nastavi Uint8 hodnotu v poli buffer
void Telegram::setByteInBuffer(int num, byte val) {
  if ((num >= 0) && (num < Telegram::BUF_LEN)) {
    byte *uint8 = (byte *)this->getBuffer();
    uint8[num] = val;
  }
}
// vrati Uint8 hodnotu v poli Buffer
byte Telegram::getByteInBuffer(int num) {
  if ((num >= 0) && (num < Telegram::BUF_LEN)) {
    byte *uint8 = (byte *)this->getBuffer();
    return uint8[num];
  }
  return 0;
}
//--------------------------------------------------------------
// prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool Telegram::isValidTelegram() {
  if (this->msg.stx != Telegram::STX)
    return false;
  if (this->msg.etx != Telegram::ETX)
    return false;
  return true;
}

word Telegram::getUint16(int num) {
  if ((num < 10) && (num >= 0)) {
    word *uint16 = (word *)this->getTelegram();
    uint16 += 2;
    return uint16[num];
  } else {
    return 0;
  }
}

void Telegram::setUint16(int num, word val) {
  if ((num < 10) && (num >= 0)) {
    word *uint16 = (word *)this->getTelegram();
    uint16 += 2;
    uint16[num] = val;
  }
}
// prekonvertuje telegram do char retazca ukonceneho '\x00'
// buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
// buffer - pole znakov kam sa bude konvertovat
// msg - struktura telegramu
// len - velkost struktury telegramu v bajtoch; sizeof(TELEGRAM)
void Telegram::encodeTelegram() {
  byte *b_msg = (byte *)this->getTelegram();
  byte *buf = (byte *)this->getBuffer();
  byte a, b, c;
  int j = 0;

  for (int i = 0; i < Telegram::MSG_LEN; i++) {
    c = b_msg[i];
    a = c & 0x0F;
    b = (c >> 4) & 0x0F;
    buf[j] = (a + Telegram::ADD) & 0xFF;
    j++;
    buf[j] = (b + Telegram::ADD) & 0xFF;
    j++;
  }
  buf[j] = 0x00;
}
// prekonvertuje char retazec ukonceny '\x00' na telegram
// buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
// buffer - pole znakov odkial sa bude konvertovat
// msg - struktura telegramu
// len - velkost struktury telegramu v bajtoch; int len = sizeof(TELEGRAM);
void Telegram::decodeTelegram() {
  byte *b_msg = (byte *)this->getTelegram();
  byte *buf = (byte *)this->getBuffer();
  byte a, b;

  for (int i = 0, j = 0; i < Telegram::MSG_LEN; i++) {
    a = (buf[j] - Telegram::ADD) & 0x0F;
    j++;
    b = (buf[j] - Telegram::ADD) & 0x0F;
    j++;
    b_msg[i] = a | (b << 4);
  }
}