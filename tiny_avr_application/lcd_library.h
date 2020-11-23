/*
 * lcd_library.h
 *
 * Created: 3/05/2020 4:43:21 PM
 *  Author: lance
 */ 


#ifndef LCD_LIBRARY_H_
#define LCD_LIBRARY_H_

#include <util/delay.h>

#define CS_PIN 3
#define DC_PIN 4
#define BUTTON 5

#define PORTRAIT_MAX_X 240
#define PORTRAIT_MAX_Y 320

#define CS_HIGH PORTB |= (1<<CS_PIN);
#define DC_HIGH PORTB |= (1<<DC_PIN);
#define CS_LOW PORTB &= ~(1<<CS_PIN);
#define DC_LOW PORTB &= ~(1<<DC_PIN);

void spi_setup();
void lcd_sendCmd(unsigned char data);
void lcd_sendData(unsigned char data);
void lcd_sendData16(unsigned int data);
void lcd_fillRectangle(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, unsigned int color);
void lcd_setX(unsigned int x0, unsigned int x1);
void lcd_setY(unsigned int y0, unsigned int y1);
void lcd_hline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color);
void lcd_init();
void lcd_fillRectangleSideToSide(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, int pixels_to_fill, unsigned int color); 
void lcd_fillRectangleTopToBottom(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, int pixels_to_fill, unsigned int color); 
void lcd_vline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color);
void draw_bitmap(const uint16_t * ptr_to_bitmap,unsigned int x, unsigned int y, unsigned int size_x, unsigned int size_y,uint16_t color);
void lcd_drawPixel(unsigned int x0, unsigned int y0, unsigned int color);
void draw_expanded_bitmap(const uint16_t * ptr_to_bitmap, unsigned int x, unsigned int y,unsigned int size_x, unsigned int size_y, unsigned int pixels,uint16_t color);
#endif /* LCD_LIBRARY_H_ */