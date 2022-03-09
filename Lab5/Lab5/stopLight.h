/*
 * stopLight.h
 *
 * Created: 2021-03-14 21:36:54
 *  Author: ailio
 */ 
#include "TinyTimber.h"
#include "outHandler.h"

#ifndef STOPLIGHT_H_
#define STOPLIGHT_H_

typedef struct {
	Object super;
	outH *output;
	int status;	//0 = red, 1 = green
	int pos; //0 = north, 1 = south
} light;
void green(light *self);
void red(light *self);
void write(light *self);
#define initlight(arg, arg2) {initObject(), arg, 0, arg2};


#endif /* STOPLIGHT_H_ */