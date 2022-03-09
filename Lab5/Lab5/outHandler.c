/*
 * outHandler.c
 *
 * Created: 2021-03-14 14:37:44
 *  Author: ailio
 */ 
#include "outHandler.h"

#define FOSC 8000000
#define BAUD 9600
#define MYUBRR ((FOSC)/(16*BAUD)-1)
/*	
0 -> North green light
1 -> North red light
2 -> South green light
3 -> South red light
*/
void transmit(outH *self, int val) {
	
	while( !(UCSR0A & (1<<UDRE0)) );	//Wait for empty buffer
	
	UDR0 = val;	//"send" data
}

void initUSART(outH *self) {
	//Set baud rate
	UBRR0H = (unsigned char)(51>>8); 
	UBRR0L = (unsigned char) 51;
	//Enable transmit and receive
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
}