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

void closeRoad(TC *self, int val) {
	ASYNC(self->lights[val], red, 0);
	AFTER(SEC(1), self, clearBridge, 0);	// Adds 1 second of full red time before letting cars through again.
}

void clearBridge(TC *self) {
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
	
	if(self->onBridge == 0 && self->bridgeClear == 0) { 			// Safety check to make sure nothing is occupying the bridge (and the extra 1 second time has passed "bridgeClear")
		if(self->queues[0] == 0 && self->queues[1] > 0) {			// If one side has 0 cars while the other one has a queue open the corresponding side 
			time = self->queues[1];									// no matter which side opened last time
			self->northLast = 0;
			self->southLast = 1;
			openSouth(self, time);
		} else if(self->queues[1] == 0 && self->queues[0] > 0) {
			time = self->queues[0];
			self->northLast = 1;
			self->southLast = 0;
			openNorth(self, time);
		} else if(self->northLast == 0){							// Alternate opening north and sound sides, "fairest" and simplest approach I could come up
			time = self->queues[0];
			self->northLast = 1;
			self->southLast = 0;
			openNorth(self, time);
		} else if (self->southLast == 0) {
			time = self->queues[1];
			self->northLast = 0;
			self->southLast = 1;
			openSouth(self, time);
		} else {
			if(self->queues[0] >= self->queues[1] && self->queues[0] > 0) {				// This last "else" would only run on the first bridge opening (or if queues are empty),  
				time = self->queues[0];													// after which we will have a value of which side opened previously
				self->northLast = 1;													// Simply the one which has the biggest queue, (north if both are equal)
				self->southLast = 0;
				openNorth(self, time);
			} else if(self->queues[1] >= self->queues[0] && self->queues[1] > 0) {
				time = self->queues[1];
				self->northLast = 0;
				self->southLast = 1;
				openSouth(self, time);
			}
		}
	}
	AFTER(MSEC(500), self, controlling, 0);							// Keep running function with 0.5s intervals
}

void startup(TC *self) {
		ASYNC(&(self->display[0]), updateDisplay, self->queues[0]);
		ASYNC(&(self->display[1]), updateDisplay, self->onBridge);
		ASYNC(&(self->display[2]), updateDisplay, self->queues[1]);
		controlling(self);
}
