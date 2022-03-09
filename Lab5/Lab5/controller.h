/*
 * controller.h
 *
 * Created: 2021-03-14 21:46:47
 *  Author: ailio
 */ 
#include "TinyTimber.h"
#include "stopLight.h"
#include "GUI.h"

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

typedef struct {
	Object super;
	gui display[3];		//{left, center, right}
	light *lights[2];	//{north, south}
	int queues[2];		//{north, south}
	int onBridge;
	int bridgeClear;
	int northLast;
	int southLast;
} TC;
#define initTC(l1, l2) {initObject(), {initGUI(4), initGUI(2), initGUI(0)}, {l1, l2}, {0, 0}, 0, 0, 0, 0}
void addCar(TC *self, int val);
void removeCar(TC *self, int val);
void entry(TC *self, int val);
void removeEntry(TC *self);
void closeRoad(TC *self, int val);
void openNorth(TC *self, int val);
void openSouth(TC *self, int val);
void controlling(TC *self);
void startup(TC *self);
#endif /* CONTROLLER_H_ */