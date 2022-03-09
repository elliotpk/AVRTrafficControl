/*
 * GUI.h
 *
 * Created: 2021-03-02 21:56:33
 *  Author: ailio
 */ 
#ifndef GUI_H_
#define GUI_H_

#include "TinyTimber.h"
#include "LCD_driver.h"
#include <avr/io.h>

typedef struct {
	Object super;
	int number;
} gui;
#define initGUI(arg) {initObject(), arg}
void updateDisplay(gui *self, int val);

#endif /* GUI_H_ */