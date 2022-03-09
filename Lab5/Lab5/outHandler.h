/*
 * outHandler.h
 *
 * Created: 2021-03-14 14:37:54
 *  Author: ailio
 */ 
#include "TinyTimber.h"
#include <avr/io.h>

#ifndef OUTHANDLER_H_
#define OUTHANDLER_H_

typedef	struct {
	Object super;
} outH;
void transmit(outH *self, int val);
void initUSART(outH *self);
#define initoutH(void) {initObject()}


#endif /* OUTHANDLER_H_ */