/*
 * stopLight.c
 *
 * Created: 2021-03-14 21:36:43
 *  Author: ailio
 */ 
#include "stopLight.h"

void green(light *self) {
	self->status = 1;
	write(self);
}

void red(light *self) {
	self->status = 0;
	write(self);
}

void write(light *self) {
	if(self->pos == 0) {	//North light
		switch(self->status){
			case 1:
				ASYNC(self->output, transmit, 1);	//North green light
				break;
			case 0:
				ASYNC(self->output, transmit, 2);	//North red light
		}
	} else {				//South light
		switch(self->status){
			case 1:
				ASYNC(self->output, transmit, 4);	//South green light
				break;
			case 0:
				ASYNC(self->output, transmit, 8);	//South red light
				break;
		}
	}
}