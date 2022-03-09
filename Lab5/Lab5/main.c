/*
 * Lab5.c
 *
 * Created: 2021-03-14 21:34:47
 * Author : ailio
 */ 

#include <avr/io.h>
#include "TinyTimber.h"
#include "outHandler.h"
#include "controller.h"
#include "stopLight.h"
#include "intHandler.h"
#include "LCD_driver.h"
#include <stdint.h>

int main(void)
{
	outH out = initoutH();
	light nLight = initlight(&out, 0);
	light sLight = initlight(&out, 1);
	TC cont = initTC(&nLight, &sLight);
	intH interr = initintH(&cont);
	CLKPR = 0x80;
	CLKPR = 0x00;
	LCD_Init();
	initUSART(&out);
	INSTALL(&interr, comInterrupt, IRQ_USART0_RX);
 	INSTALL(&interr, comInterrupt, IRQ_USART0_TX);
 	return TINYTIMBER(&cont, startup, 0);
}

