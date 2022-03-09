/*
 * LCD_driver.h
 *
 * Created: 2021-01-30 16:50:38
 *  Author: ailio
 */ 

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

void writeChar(char ch, int pos);
void writeLong(long i);
void LCD_clear(void);
void LCD_Init(void);
void printAt(long num, int pos);
#endif /* LCD_DRIVER_H_ */
