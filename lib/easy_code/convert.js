const STX = 2;
const ETX = 3;

var TELEGRAM = new ArrayBuffer(26);

var data = new DataView(TELEGRAM);
data.setInt8(0, STX);
data.setInt8(25, ETX);

bool function isValidTelegram(msg){
	var data = new DataView(msg);
	if(data.getInt8(0) !== STX) 
		return false;
	if(data.getInt8(25) !== ETX) 
		return false;
	return true;
}

console.log(isValidTelegram(TELEGRAM));

//celkova bajtova velkost telegramu je: 1*2 + 4*4 + 4*2 = 26 bytov


typedef struct {
	byte STX; //allways STX
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
	byte ETX; //allways ETX
} TELEGRAM;

//-----------------------------------------------------------
//prekontroluje ci je telegram validny
// vrati true ak je validny, false ak nevalidny
bool isValidTelegram(const TELEGRAM *msg){
	if(msg.STX != STX) 
		return false;
	if(msg.ETX != ETX) 
		return false;
	return true;
}


var data =  [64, 226, 157, 10];

// Create a buffer
var buf = new ArrayBuffer(4);
// Create a data view of it
var view = new DataView(buf);

// set bytes
data.forEach(function (b, i) {
    view.setUint8(i, b);
});

// Read the bits as a float; note that by doing this, we're implicitly
// converting it from a 32-bit float into JavaScript's native 64-bit double
var num = view.getFloat32(0);
// Done
console.log(num);


// create an ArrayBuffer with a size in bytes
var buffer = new ArrayBuffer(16);

var view = new DataView(buffer);
view.setUint8(1, 255); // (max unsigned 8-bit integer)

console.log(view.getUint8(1));
// expected output: 255

// create an ArrayBuffer with a size in bytes
var buffer = new ArrayBuffer(16);

var view = new DataView(buffer);
view.setFloat32(1, Math.PI);

console.log(view.getFloat32(1));
// expected output: 3.1415927410125732


https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView/setFloat32