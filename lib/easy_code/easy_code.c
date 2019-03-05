#include <easy_code.h>


//-----------------------------------------------------------
//prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool isValidTelegram(const TELEGRAM *msg){
	if(msg->stx != STX)
		return false;
	if(msg->etx != ETX)
		return false;
	return true;
}

//prekonvertuje telegram do char retazca ukonceneho '\x00'
//buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
//buffer - pole znakov kam sa bude konvertovat
//msg - struktura telegramu
//len - velkost struktury telegramu v bajtoch; sizeof(TELEGRAM)
void encodeTelegram(byte *buffer, const TELEGRAM *msg, const int len){
	byte *b_msg = (byte *)msg;
	byte *buf = buffer;
  byte a,b;

	for(int i = 0; i < len; i++, b_msg++){
    a = *b_msg & 0x0F;
    b = (*b_msg & 0xF0) >> 4;
    *buf++ = a + ADD;
    *buf++ = b + ADD;
	}
  *buf = '\x00';
}

//prekonvertuje char retazec ukonceny '\x00' na telegram
//buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
//buffer - pole znakov odkial sa bude konvertovat
//msg - struktura telegramu
//len - velkost struktury telegramu v bajtoch; int len = sizeof(TELEGRAM);
void decodeTelegram(TELEGRAM *msg, const int len, const byte *buffer){
  byte *b_msg = (byte *)msg;
  byte *buf = buffer;
  byte a,b;

  for(int i = 0; i < len; i++, b_msg++){
    a = *buf++ - ADD;
    b = *buf++ - ADD;
    *b_msg = (a & 0x0F) | ((b & 0x0F)<<4);
  }
}
