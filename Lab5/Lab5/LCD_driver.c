/*
 * LCD_driver.c
 *
 * Created: 2021-01-22 00:38:49
 *  Author: Elliot Palokangas Karlsson
 */ 
#include "LCD_driver.h"
#include <avr/io.h>
#include <stdint.h>

//Do (char - '0') to get index
static const uint16_t numberLookUp[10] = {
	0x1551, 0x118, 0x1E11, 0x1911, 0xB50, 0x1B41,
	0x1F41, 0x2009, 0x1F51, 0xB51
};

//Do (char - 'A') to get index
static const uint16_t letterLookUp[26] = {
	0xF51, 0x3991, 0x1441, 0x3191, 0x1641, 0x641, 0x1D41,
	0xF50, 0x3081, 0x1510, 0x8648, 0x1440, 0x578,
	0x8570, 0x1551, 0xE51, 0x9551, 0x8E51, 0x1B41,
	0x2081, 0x1550, 0x4448, 0xC550, 0xC028, 0x2028,
	0x5009
};

/*
Do checks to make sure the requested position is valid
Check that the character requested is supported, lowercase letters converted to uppercase
Fetch the segment control code from tables and decide on which nibble we need.
*/
void writeChar(char ch, int pos) {
	if(pos<0 || pos >5) {
		return;
	}
	
	uint16_t scc;

	if(ch > 0x60 && ch < 0x7B) {//If ASCII a-z (convert to uppercase)
		scc = letterLookUp[((ch-32)-'A')];
	} else if(ch > 0x2F && ch < 0x3A) {//If ASCII 0-9
		scc = numberLookUp[(ch-'0')];
	} else if (ch > 0x40 && ch < 0x5B) {//If ASCII A-Z
		scc = letterLookUp[(ch-'A')];
	} else {
		return;
	}
	uint8_t *ptr;
	uint8_t mask;
	
	switch(pos) {
		case 0:
		case 1:
			ptr = &LCDDR0;
			break;
		case 2:
		case 3:
			ptr = &LCDDR1;
			break;
		case 4:
		case 5:
			ptr = &LCDDR2;
			break;
	}

	if(pos % 2 == 0) {				//Even -> lower nibble
		mask = 0xF0;
	} else {						//Odd -> upper nibble
		mask = 0x0F;
	}
	
	for(int i = 0; i < 4; i++) {
		uint8_t val = scc & 0xF;
		scc = scc >> 4;
		if(pos % 2 != 0) {
			val = val << 4;
		}
		*ptr = (*ptr & mask) | val;
		ptr += 5;
	}
}

void writeLong(long i) {
	LCD_clear();
	char arr[6];
	int j = 0;
	
	while(i != 0){
		arr[j] = (i % 10)+'0';
		i = i/10;
		j++;
	}
	writeChar(arr[5], 0);
	writeChar(arr[4], 1);
	writeChar(arr[3], 2);
	writeChar(arr[2], 3);
	writeChar(arr[1], 4);
	writeChar(arr[0], 5);
}

void LCD_clear(void) {
	uint8_t *ptr0 = &LCDDR0;
	uint8_t *ptr1 = &LCDDR1;
	uint8_t *ptr2 = &LCDDR2;
	*ptr0 &= 0x40;
	*ptr1 &= 0b100;
	*ptr2 &= 0b100;
			
	ptr0 += 5;
	ptr1 += 5;
	ptr2 += 5;
	for(int i = 0; i < 3; i++) {
		*ptr0 &= 0x00;
		*ptr1 &= 0x00;
		*ptr2 &= 0x00;
		
		ptr0 += 5;
		ptr1 += 5;
		ptr2 += 5;
	}
}

void LCD_Init(void){
	//LCD Control and Status Register B
	//External clock, 1/3 bias, 1/4 duty, 25segment
	//Bit 7, 5, 4, 2, 1, 0
	LCDCRB = (1<<LCDCS) | (1<<LCDMUX1) | (1<<LCDMUX0) | (1<<LCDPM2) | (1<<LCDPM1) | (1<<LCDPM0);
	
	//LCD Frame Rate Register
	//N=16 D=8 
	//Bit 2, 1, 0
	LCDFRR = (1<<LCDCD2) | (1<<LCDCD1) | (1<<LCDCD0);
	
	//LCD Contrast Control Register
	//Set to 3.35V with LCDCC3:0 registers
	//Bit 3, 2, 1, 0
	LCDCCR = (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1) | (1<<LCDCC0);
	
	//LCD Control and Status Register A
	//LCD & low power waveform enabled
	//Bit 7, 6
	LCDCRA = (1<<LCDEN) | (1<<LCDAB);
}
//Register bits^ page 218-223

void printAt(long num, int pos) {
	int pp = pos;
	writeChar( (num % 100) / 10 + '0', pp);
	pp++;
	writeChar( num % 10 + '0', pp);
}

/*
Register|Bit7	6	5	4	3	2	1	0
LCDDRx		K	-	-	A	K	-	-	A
LCDRRx+5	J	F	H	B	J	F	H	B
LCDRRx+10	L	E	G	C	L	E	G	C
LCDRRx+15	M	P	N	D	M	P	N	D


Even positions use 3:0(lower nibble),		Odd positions use 4:7 (upper nibble)
digit 2,3 : x=0
digit 4,5 : x=1
digit 6,7 : x=2
*/