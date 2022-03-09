/*
 * intHandler.h
 *
 * Created: 2021-03-16 12:25:17
 *  Author: ailio
 */ 
#include "TinyTimber.h"
#include "controller.h"

#ifndef INTHANDLER_H_
#define INTHANDLER_H_

typedef struct{
	Object super;
	TC *controlller;
} intH;
void comInterrupt(intH *self);
#define initintH(arg) {initObject(), arg}

#endif /* INTHANDLER_H_ */