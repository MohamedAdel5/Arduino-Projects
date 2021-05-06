//Name: Mohamed Adel Abdelmohsen
//Sec: 2
//BN: 23
//Semester
byte col1 = 8;	//Pin number of column 1
byte col2 = 9;	//Pin number of column 2
byte col3 = 10;	//Pin number of column 3
byte numbers_array[16][3] = {{0x7F, 0x41, 0x7F},{0x7F, 0, 0},{0x79, 0x49, 0x4F},{0x49,0x49,0x7F},{0x0F,0x08,0x7F},{0x4F,0x49,0x79}, {0x7F, 0x49,0x79}, {0x01, 0x01, 0x7F}, {0x7F, 0x49, 0x7F},{0x4F, 0x49, 0x7F}, {0x7F, 0x09, 0x7F}, {0x7F, 0x48, 0x78},{0x7F, 0x41,0x41}, {0x7F, 0x22, 0x1C},{0x7F, 0x49, 0x49}, {0x7F, 0x09, 0x09}};
#define DELAY_VALUE 20
void setup() {
	//DIRECTLY ACCESSING PORTS. (Less code)
	//Configure 7 output pins FOR rows output.
	//Pins 0,1,2,3,4,5,6 in arduino
	DDRD = 0xFF;	//SET ALL PORTD pins AS OUTPUT
	//Configure 3 output pins FOR columns enable.
	//Pins 8,9,10 in arduino
	DDRB |= 0x07;	//Set the first four pins in PORTB as output pins
	//Configure 4 input pins FOR the switches.
	//Pins A2,A3,A4,A5 in arduino
	DDRC &= 0xC3;	//Set four pins in PORTC as input pins
	PORTC |= 0x3C;	//Enable the pullup resistors for the four input pins in PORTC
}
void loop() {
	//GET the number from the switches
	byte number = (~((PINC & 0x3C) >> 2))&0x0F;
	for(byte i = 1; i <= 3; ++i){
		selectColumn(i);
		PORTD = numbers_array[number][i-1];
		delay(DELAY_VALUE);
	}
}
void selectColumn(byte col)
{
	digitalWrite(col1, ((col == 1)?0:1));
	digitalWrite(col2, ((col == 2)?0:1));
	digitalWrite(col3, ((col == 3)?0:1));
}