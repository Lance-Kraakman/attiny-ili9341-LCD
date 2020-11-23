/*
 * tiny_avr_application.c
 *
 * Created: 29/04/2020 8:40:49 PM
 * Author : lance
	
	initial design is for portrait orientation 
 
 */ 

#include <avr/io.h>
#include "lcd_library.h"
#include "colors.h"

#define BUTTON_PRESSED (!(PINB) && (1<<BUTTON))

void display_1(); 
void display_2();
void display_3();
void display_4(); 

extern uint16_t image[];

void setup() {
	
	// setup the timer
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	PORTB = 0x00;
	DDRB |= (1<<CS_PIN)|(1<<DC_PIN); //spi board logic pins
	
	DDRB &= ~(1<<BUTTON); //set up the button input pin as an input
	MCUCR &= ~(1<<PUD); //make sure the pull up disable is 0, enabling the pullup 
	PORTB |= (1<<5); //turn on the internal pull up resistor for the button. 

}

int main(void)
{
	
	/* Replace with your application code */
	setup();
	spi_setup();
	//we need to implement the link below! well some of it atleast :)
	//https://github.com/thefallenidealist/ili9341/blob/master/glcd.c
	
	lcd_init();

	
	
	lcd_drawPixel(0,0,RED);
	
	//display image
	display_4();

	

	
	while (1) {
	
	}

}


void display_1() {
	lcd_fillRectangle(0,0,PORTRAIT_MAX_X,PORTRAIT_MAX_Y, BLUE_NICE);
}

void display_2() {
	lcd_fillRectangleTopToBottom(0,0,PORTRAIT_MAX_X,PORTRAIT_MAX_Y,PORTRAIT_MAX_Y-250, BLACK);
}

void display_3() {
	lcd_fillRectangleSideToSide(0,0,PORTRAIT_MAX_X,PORTRAIT_MAX_Y, PORTRAIT_MAX_X-150, BLACK);
}


void display_4() {
	//draw_bitmap(&image[0], 50, 50, 50, 50,RED);
	draw_expanded_bitmap(&image[0], 50, 50, 50, 50,4,RED);
}




