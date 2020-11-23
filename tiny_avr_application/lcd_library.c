/*
 * lcd_library.h
 *
 * Created: 3/05/2020 4:43:30 PM
 *  Author: lance
 */ 
#include <avr/io.h>
#include "ili9341cmd.h"
#include "lcd_library.h"
#include <avr/pgmspace.h>

void spi_setup() {

	DDRB |= (1<<1)|(1<<2); //D0 of serial interface and USCK output.
	DDRB &= ~(1<<0); //DI of serial interface. (input)
	PORTB |= (1<<0); // turn on the internal pull up resistor for DI
}

// before we send data we must make DC pin on spi high or low!
static unsigned char spi_send_data(unsigned char val)
{
	USIDR = val;
	USISR = (1<<USIOIF);
	do {
		USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
	} while ((USISR & (1<<USIOIF)) == 0);
	return USIDR;
}

void lcd_sendCmd(unsigned char data) {
	DC_LOW
	CS_LOW
	spi_send_data(data);
	CS_HIGH
}

void lcd_sendData(unsigned char data) {
	DC_HIGH
	CS_LOW
	spi_send_data(data);
	CS_HIGH
}

void lcd_sendData16(unsigned int data) {
	unsigned char data1 = data >> 8;
	unsigned char data2 = data & 0xFF;
	DC_HIGH
	CS_LOW
	spi_send_data(data1);
	spi_send_data(data2);
	CS_HIGH
}

void lcd_hline(unsigned int x0, unsigned int y0, unsigned int length, unsigned int color) {
	
	lcd_setX(x0,x0 + length);
	lcd_setY(y0,y0);
	lcd_sendCmd(ILI9341_CMD_MEMORY_WRITE);
	
	int i=0;
	for(i=0; i<length; i++)
	lcd_sendData16(color);
}

void lcd_vline(unsigned int x0, unsigned int y0, unsigned int height, unsigned int color) {
	
	lcd_setX(x0,x0);
	lcd_setY(y0,y0+height);
	lcd_sendCmd(ILI9341_CMD_MEMORY_WRITE);
	
	int i = 0;
	for (i = 0; i < height; i++) {
		lcd_sendData16(color);
	}
}

void lcd_drawPixel(unsigned int x0, unsigned int y0, unsigned int color) {
	lcd_setX(x0,x0);
	lcd_setY(y0,y0);
	lcd_sendCmd(ILI9341_CMD_MEMORY_WRITE);
	lcd_sendData16(color);	
}

void lcd_fillRectangle(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, unsigned int color)
{
	for(int i=0;i<width;i++)
	lcd_hline(x0,y0+i,length,color);
}

void lcd_fillRectangleSideToSide(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width,int pixels_to_fill, unsigned int color) {
	
	int i = 0;
	int max_it = (int) pixels_to_fill/2 + 1;
	int max_length = y0 + length;
	
	while (i < max_it) {
		
		lcd_vline(x0+i,y0,width,color);
		lcd_vline(max_length-i,y0,width, color);
		i += 1;
		
	}
}

void draw_bitmap(const uint16_t * ptr_to_bitmap, unsigned int x, unsigned int y,unsigned int size_x, unsigned int size_y, uint16_t color) {
	
	uint16_t i = 0; 
	uint8_t byte = 0; 
	//bitmap of zeros and ones 
	for (int y = size_y; y > 0; y--) {
		for (int x = size_x/2; x>0; x--) {
				byte = pgm_read_byte((ptr_to_bitmap+i));
				if (byte == 1) {
					lcd_drawPixel(x+100,y+100,color);
				}
				i++;
		}
		
	}
}

//This works but we need to make it work for all situations. ie dynamicallly use the pixels varables
//look at the calculations and think about how to do this. with a selectable scale factor we call pixels

void draw_expanded_bitmap(const uint16_t * ptr_to_bitmap, unsigned int x, unsigned int y,unsigned int size_x, unsigned int size_y, unsigned int pixels,uint16_t color) {
		uint16_t i = 0;
		uint8_t byte = 0;
		//bitmap of zeros and ones
		for (int y = size_y*2; y > 0; y -= 2) {
			for (int x = ((size_x*2)/2); x>0; x -= 2) {
				byte = pgm_read_byte((ptr_to_bitmap+i));
				if (byte == 1) {
					lcd_drawPixel(x+100,y+100,color);
					lcd_drawPixel(x+101,y+100,color);
					lcd_drawPixel(x+100,y+101,color);
					lcd_drawPixel(x+101,y+101,color);
				}
				i++;
			}
			
		}
}



/*
void draw_bitmap(const uint16_t * ptr_to_bitmap,unsigned int x, unsigned int y, unsigned int size_x, unsigned int size_y) {
	
	uint16_t bytewidth = (size_x + 7) / 8; 
	uint16_t byte;
	 
	for (int j = 0; j < size_x - 1; j++) {
		for (int i = 0;i < size_y; i++) {
			if (i & 7) {
				byte <<= 1; 
			} else {
				byte = pgm_read_word((ptr_to_bitmap+j * bytewidth + i / 8));
			}
			if (byte & 0x80) {
				lcd_drawPixel(x+i, y+j, 0xFFFF);
			}
		}
	}
}
*/

void lcd_fillRectangleTopToBottom(unsigned int x0, unsigned int y0, unsigned int length, unsigned int width, int pixels_to_fill, unsigned int color) {
	
	int i = 0; 
	int max_it = (int) pixels_to_fill/2 + 1; 
	int max_height = x0 + width; 
	
	while (i < max_it) {
	
		lcd_hline(x0,y0+i,length,color); 
		lcd_hline(x0,max_height-i,length, color); 
		i += 1;  
	
	}
	
}

void lcd_setX(unsigned int x0, unsigned int x1) {
	lcd_sendCmd(ILI9341_CMD_COLUMN_ADDRESS_SET);
	lcd_sendData16(x0);
	lcd_sendData16(x1);
}

void lcd_setY(unsigned int y0, unsigned int y1) {
	lcd_sendCmd(ILI9341_CMD_PAGE_ADDRESS_SET);
	lcd_sendData16(y0);
	lcd_sendData16(y1);
}

void lcd_init() {
	
	CS_HIGH
	DC_HIGH
	lcd_sendCmd(0x01);
	_delay_ms(10);
	
	lcd_sendCmd(ILI9341_CMD_POWER_ON_SEQ_CONTROL);
	lcd_sendData(ILI9341_CMD_IDLE_MODE_ON);
	lcd_sendData(ILI9341_CMD_MEMORY_WRITE);
	lcd_sendData(ILI9341_CMD_NOP);
	lcd_sendData(ILI9341_CMD_TEARING_EFFECT_LINE_OFF);
	lcd_sendData(0x02); 	// XXX

	lcd_sendCmd(ILI9341_CMD_POWER_CONTROL_B);
	lcd_sendData(ILI9341_CMD_NOP);
	lcd_sendData(ILI9341_CMD_POWER_CONTROL_2);
	lcd_sendData(ILI9341_CMD_PARTIAL_AREA);

	lcd_sendCmd(ILI9341_CMD_DRIVER_TIMING_CONTROL_A);
	lcd_sendData(0x85); 	// XXX
	lcd_sendData(ILI9341_CMD_NOP);
	lcd_sendData(0x78); 	// XXX

	lcd_sendCmd(ILI9341_CMD_DRIVER_TIMING_CONTROL_B);
	lcd_sendData(ILI9341_CMD_NOP);
	lcd_sendData(ILI9341_CMD_NOP);

	lcd_sendCmd(0xED);	// XXX
	lcd_sendData(0x64); 	// XXX
	lcd_sendData(0x03);	// XXX
	lcd_sendData(ILI9341_CMD_PARTIAL_MODE_ON);
	lcd_sendData(0X81); 	// XXX

	lcd_sendCmd(ILI9341_CMD_PUMP_RATIO_CONTROL);
	lcd_sendData(ILI9341_CMD_DISP_INVERSION_OFF);

	lcd_sendCmd(ILI9341_CMD_POWER_CONTROL_1);
	lcd_sendData(0x23);	//VRH[5:0] 	// XXX

	lcd_sendCmd(ILI9341_CMD_POWER_CONTROL_2);
	lcd_sendData(ILI9341_CMD_ENTER_SLEEP_MODE);
	

	lcd_sendCmd(ILI9341_CMD_VCOM_CONTROL_1);
	lcd_sendData(ILI9341_CMD_READ_MEMORY_CONTINUE);
	lcd_sendData(ILI9341_CMD_DISPLAY_OFF);

	lcd_sendCmd(ILI9341_CMD_VCOM_CONTROL_2);
	lcd_sendData(0x86);	//--	// XXX

	lcd_sendCmd(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
	lcd_sendData(0x48);	//C8	//48 68gal.gal.gal.//28 E8 gal.gal.gal.	// XXX

	lcd_sendCmd(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET);
	lcd_sendData(ILI9341_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS);

	lcd_sendCmd(ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL);
	lcd_sendData(ILI9341_CMD_NOP);
	lcd_sendData(0x18); 	// XXX

	lcd_sendCmd(ILI9341_CMD_DISPLAY_FUNCTION_CONTROL);
	lcd_sendData(0x08); 	// XXX
	lcd_sendData(0x82);	// XXX
	lcd_sendData(0x27);	// XXX

	lcd_sendCmd(ILI9341_CMD_ENABLE_3_GAMMA_CONTROL);
	lcd_sendData(ILI9341_CMD_NOP);

	lcd_sendCmd(0x26);	//Gamma curve selected 	// XXX
	lcd_sendData(ILI9341_CMD_SOFTWARE_RESET);

	lcd_sendCmd(ILI9341_CMD_POSITIVE_GAMMA_CORRECTION);
	lcd_sendData(0x0F); 	// XXX
	lcd_sendData(0x31);	// XXX
	lcd_sendData(ILI9341_CMD_PAGE_ADDRESS_SET);
	lcd_sendData(ILI9341_CMD_READ_DISP_PIXEL_FORMAT);
	lcd_sendData(ILI9341_CMD_READ_DISP_SIGNAL_MODE);
	lcd_sendData(0x08); 	// XXX
	lcd_sendData(0x4E); 	// XXX
	lcd_sendData(0xF1); 	// XXX
	lcd_sendData(ILI9341_CMD_VERT_SCROLL_START_ADDRESS);
	lcd_sendData(0x07); 	// XXX
	lcd_sendData(ILI9341_CMD_ENTER_SLEEP_MODE);
	lcd_sendData(0x03);	// XXX
	lcd_sendData(ILI9341_CMD_READ_DISP_SIGNAL_MODE);
	lcd_sendData(ILI9341_CMD_READ_DISP_STATUS);
	lcd_sendData(ILI9341_CMD_NOP);

	lcd_sendCmd(ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION);
	lcd_sendData(ILI9341_CMD_NOP);
	lcd_sendData(ILI9341_CMD_READ_DISP_SIGNAL_MODE);
	lcd_sendData(0x14); 	// XXX
	lcd_sendData(0x03);	// XXX
	lcd_sendData(ILI9341_CMD_SLEEP_OUT);
	lcd_sendData(0x07); 	// XXX
	lcd_sendData(0x31); 	// XXX
	lcd_sendData(ILI9341_CMD_POWER_CONTROL_2);
	lcd_sendData(0x48); 	// XXX
	lcd_sendData(0x08); 	// XXX
	lcd_sendData(0x0F); 	// XXX
	lcd_sendData(ILI9341_CMD_READ_DISP_PIXEL_FORMAT);
	lcd_sendData(0x31); 	// XXX
	lcd_sendData(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
	lcd_sendData(ILI9341_CMD_READ_DISP_SELF_DIAGNOSTIC);
	lcd_sendCmd(ILI9341_CMD_SLEEP_OUT);
	_delay_ms(120);

	lcd_sendCmd(ILI9341_CMD_DISPLAY_ON);
	lcd_sendCmd(ILI9341_CMD_MEMORY_WRITE);
	
}