/*
 * controller.c
 *
 * Created: 2021-03-14 21:46:41
 *  Author: ailio
 */ 
#include "controller.h"

void addCar(TC *self, int val) {
	if(val == 0) {
		self->queues[0]++;
		ASYNC(&(self->display[0]), updateDisplay, self->queues[0]);
	} else {
		self->queues[1]++;
		ASYNC(&(self->display[2]), updateDisplay, self->queues[1]);
	}
}

void removeCar(TC *self, int val) {
	if(val == 0) {
		self->queues[0]--;
		ASYNC(&(self->display[0]), updateDisplay, self->queues[0]);
	} else {
		self->queues[1]--;
		ASYNC(&(self->display[2]), updateDisplay, self->queues[1]);
	}
}

void entry(TC *self, int val) {	//0 = north entry, 1 = south entry
	self->onBridge++;
	ASYNC(&(self->display[1]), updateDisplay, self->onBridge);
	AFTER(SEC(5), self, removeEntry, 0);
	removeCar(self, val);
}

void removeEntry(TC *self) {
	self->onBridge--;
	ASYNC(&(self->display[1]), updateDisplay, self->onBridge);
}

/* void openRoad(TC *self, int val) {
	int closetime = 0;
	if(self->bridgeClear!=0) {
		return;
	}
	self->bridgeClear = 1;
	closetime = 5*self->queues[val];
	ASYNC(self->lights[val], green, 0);
	if(self->queues[val] > 0) {
		AFTER(SEC(closetime), self, closeRoad, val);
	} else {
		closeRoad(self, 0);
	}
} */

void closeRoad(TC *self, int val) {
	ASYNC(self->lights[val], red, 0);
	self->bridgeClear = 0;
}
 
 void openNorth(TC *self, int val) {
	 self->bridgeClear = 1;
	 ASYNC(self->lights[0], green, 0);
	 AFTER(SEC(val), self, closeRoad, 0);
}

void openSouth(TC *self, int val) {
	 self->bridgeClear = 1;
	 ASYNC(self->lights[1], green, 0);
	 AFTER(SEC(val), self, closeRoad, 1);
}
 
void controlling(TC *self) {
	int time = 0;
	if(self->onBridge == 0) {
		if(self->bridgeClear == 0) {
			if(self->queues[0] >= self->queues[1]) {
				if(self->queues[0] > 0 && self->northLast == 0){
					time = self->queues[0];
					self->northLast = 1;
					self->southLast = 0;
					openNorth(self, time);
				}
			} else if(self->queues[1] >= self->queues[0]) {
				if(self->queues[1] > 0 && self->southLast == 0){
					time = self->queues[1];
					self->southLast = 1;
					self->northLast = 0;
					openSouth(self, time);
				}
			} else if(self->queues[1] == 0 && self->queues[0] > 0) {
					time = self->queues[0];
					self->northLast = 1;
					self->southLast = 0;
					openNorth(self, time);
			} else {
					time = self->queues[1];
					self->southLast = 1;
					self->northLast = 0;
					openSouth(self, time);
			}				
		}		
	}
	AFTER(MSEC(500), self, controlling, 0);//Keep running function with 0.5s intervals
}

//FINAL BURST: Make sure the function above opens the correct side. Try to alternate sides to make it fair, if one is 0 open the other (if that is not 0) ETC 

void startup(TC *self) {
		ASYNC(&(self->display[0]), updateDisplay, self->queues[0]);
		ASYNC(&(self->display[1]), updateDisplay, self->onBridge);
		ASYNC(&(self->display[2]), updateDisplay, self->queues[1]);
		controlling(self);
}
