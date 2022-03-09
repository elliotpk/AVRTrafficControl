/*
 * GUI.c
 *
 * Created: 2021-03-02 21:56:25
 *  Author: ailio
 */ 
#include "GUI.h"

void updateDisplay(gui *self, int val) {
	printAt(val, self->number);
}