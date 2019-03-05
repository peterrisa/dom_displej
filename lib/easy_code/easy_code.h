#ifndef __EASY_CODE_HANDLER_H__
#define __EASY_CODE_HANDLER_H__

//start of telegram
#define STX 0x02
//end of telegram
#define ETX 0x03
//
#define ADD 0x41

typedef union {
	float v; 
	byte b[sizeof(float)];
} F_VALUE;

typedef union {
	struct {
		unsigned short b_00: 1;
		unsigned short b_01: 1; 
		unsigned short b_02: 1; 
		unsigned short b_03: 1; 
		unsigned short b_04: 1; 
		unsigned short b_05: 1; 
		unsigned short b_06: 1; 
		unsigned short b_07: 1; 
		unsigned short b_08: 1; 
		unsigned short b_09: 1; 
		unsigned short b_10: 1; 
		unsigned short b_11: 1; 
		unsigned short b_12: 1; 
		unsigned short b_13: 1; 
		unsigned short b_14: 1; 
		unsigned short b_15: 1;
	} v;
	byte b[sizeof(v)];
} B_VALUE;

//celkova bajtova velkost telegramu je: 1*2 + 4*4 + 4*2 = 26 bytov
typedef struct {
	byte stx; //allways STX
//-------------------------------	
//payload	
//-------------------------------
	F_VALUE v_1;//4 bytes
	F_VALUE v_2;//4 bytes
	F_VALUE v_3;//4 bytes
	F_VALUE v_4;//4 bytes
	B_VALUE b_1;//2 bytes
	B_VALUE b_2;//2 bytes
	B_VALUE b_3;//2 bytes
	B_VALUE b_4;//2 bytes	
//-------------------------------	
//end of payload	
//-------------------------------
//	byte CRC1;
//	byte CRC2;
//-------------------------------
//shoud be CRC check
//-------------------------------
	byte etx; //allways ETX
} TELEGRAM;

//prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool isValidTelegram(const TELEGRAM *msg);

//prekonvertuje telegram do char retazca ukonceneho '\x00'
//buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
//buffer - pole znakov kam sa bude konvertovat
//msg - struktura telegramu
//len - velkost struktury telegramu v bajtoch; sizeof(TELEGRAM)
void encodeTelegram(byte *buffer, const TELEGRAM *msg, const int len);

//prekonvertuje char retazec ukonceny '\x00' na telegram
//buffer musi mat velkost 2*sizeof(TELEGRAM) + 1 bajtov
//buffer - pole znakov odkial sa bude konvertovat
//msg - struktura telegramu
//len - velkost struktury telegramu v bajtoch; int len = sizeof(TELEGRAM);
void decodeTelegram(TELEGRAM *msg, const int len, const byte *buffer);



#endif
