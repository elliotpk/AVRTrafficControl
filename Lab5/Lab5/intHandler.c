/*
 * intHandler.c
 *
 * Created: 2021-03-16 12:25:09
 *  Author: ailio
 */ 
#include "intHandler.h"

void comInterrupt(intH *self) {
	while (!(UCSR0A & (1<<RXC0)));
	int read = UDR0;
	
	if(read == 1) {
		ASYNC(self->controlller, addCar, 0);
	} else if (read == 2) {
		ASYNC(self->controlller, entry, 0);
	} else if (read == 4) {
		ASYNC(self->controlller, addCar, 1);		
	} else if (read == 8) {
		ASYNC(self->controlller, entry, 1);		
	}
}