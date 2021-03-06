/* 
   Copyright (c) 2003 Joerg Mossbrucker <mossbruc@msoe.edu>
   
   All Rights Reserved.
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
  
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
  
*/

/*
	lcd.c
	contains functions to control the LCD
	Copyright (c) 2004 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _LCD_C
#define _LCD_C

#include <math.h>
#include <inttypes.h>
#include <avr/io.h>				// is needed for IO port declarations
#include <avr/interrupt.h>
#include <stdlib.h>
#include "globaldef.h"
#include "lcd.h"

// initializes LCD
//  - init PORT DDR and PINS
//  - set two line display
//  - clear display and set cursor to home
void lcd_init(void)
{
	delay_ms(100);			// wait 100ms for the LCD to come out of RESET
	lcd_port_init();		// init the port pins
	lcd_cmd_write(0x33);	// init LCD to 4-bit interface
	lcd_cmd_write(0x32);	// init LCD to 4-bit interface
	lcd_cmd_write(0x28);	// set two-line display
	lcd_cmd_write(0x0C);
	lcd_cmd_write(0x06);
	lcd_cmd_write(LCD_CLR);	// clear display
	lcd_cmd_write(LCD_HOME);
}

// initializes the ports
// sets the 4 upper bits of the data port (i.e. the LCD data is always connected
// to the upper 4 bits of a port)
// sets the control port (this could be anywhere)
void lcd_port_init(void)
{
	sbi(LCD_DATA_DDR,7);						// set pin 7-4 of LCD data port to output
	sbi(LCD_DATA_DDR,6);
	sbi(LCD_DATA_DDR,5);
	sbi(LCD_DATA_DDR,4);

	cbi(LCD_CTRL_PORT, LCD_CTRL_RS);			// clear bits of control port
	cbi(LCD_CTRL_PORT, LCD_CTRL_RW);
	cbi(LCD_CTRL_PORT, LCD_CTRL_E);


	sbi(LCD_CTRL_DDR,LCD_CTRL_RS);				// set pins of control port to output
	sbi(LCD_CTRL_DDR,LCD_CTRL_RW);
	sbi(LCD_CTRL_DDR,LCD_CTRL_E);
}

// sends a command byte to LCD
void lcd_cmd_write(uint8_t cmd)
{
	cbi(LCD_CTRL_PORT,LCD_CTRL_RS);						// lower RS -> command follows
	delay_ms(2);

	sbi(LCD_CTRL_PORT,LCD_CTRL_E);						// raise E
	LCD_DATA_PORT=(LCD_DATA_PORT&0x0F)|(cmd&0xF0);		// send upper nibble
	cbi(LCD_CTRL_PORT,LCD_CTRL_E);						// lower E
	delay_ms(2);										// wait 2ms so display uploads data
														// some commands take more than 1ms

	sbi(LCD_CTRL_PORT,LCD_CTRL_E);						// raise E
	LCD_DATA_PORT=(LCD_DATA_PORT&0x0F)|((cmd<<4)&0xF0);	// send lower nibble
	cbi(LCD_CTRL_PORT,LCD_CTRL_E);						// lower E
	delay_ms(2);										// wait 2ms so display uploads data

	sbi(LCD_CTRL_PORT,LCD_CTRL_RS);						// raise RS end of command
	delay_ms(2);
}

// sends a data byte to LCD
void lcd_data_write(uint8_t data)
{
	sbi(LCD_CTRL_PORT,LCD_CTRL_E);						// raise E
	LCD_DATA_PORT=(LCD_DATA_PORT&0x0F)|(data&0xF0);		// send upper nibble
	cbi(LCD_CTRL_PORT,LCD_CTRL_E);						// lower E
	delay_ms(2);										// wait 1ms so display uploads data

	sbi(LCD_CTRL_PORT,LCD_CTRL_E);						// raise E
	LCD_DATA_PORT=(LCD_DATA_PORT&0x0F)|((data<<4)&0xF0);// send lower nibble
	cbi(LCD_CTRL_PORT,LCD_CTRL_E);						// lower E
	delay_ms(2);										// wait 1ms so display uploads data
}

// sets LCD cursor to position x,y
void lcd_goto_xy(uint8_t x,uint8_t y)
{
	uint8_t DDRAMAddr;
	 
	switch(y)							// get the right starting address for the line
	{									// and add offset for x position
		case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
		case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
		case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
		case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
		default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcd_cmd_write(1<<LCD_DDRAM | DDRAMAddr);
}

// clears LCD display
void lcd_clear(void)
{
	lcd_cmd_write(LCD_CLR);		// clear display
}

// sets LCD cursor to home
void lcd_home(void)
{
	lcd_cmd_write(LCD_HOME);	// cursor home
}

// prints a char
void lcd_print_char(uint8_t symbol)
{
	lcd_data_write(symbol);
}

// prints a null-terminated string on LCD
void lcd_print_string(char *string)
{
	while(*string!=0)
	{
		lcd_data_write(*string);
		string++;
	}
}

// prints hex number on LCD
void lcd_print_hex(uint8_t hex)
{
	uint8_t hi,lo;

	hi=hex&0xF0;
	hi=hi>>4;
	hi=hi+'0';
	if(hi>'9')
	{
		hi=hi+7;
	}
	lo=hex&0x0F;
	lo=lo+'0';
	if(lo>'9')
	{
		lo=lo+7;
	}

	lcd_data_write(hi);
	lcd_data_write(lo);
}

// print uint8 on LCD
void lcd_print_uint8(uint8_t no)
{
	uint8_t y,leading;
	leading=1;
	y=no/100;
	if(y>0)
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	no=no-y*100;
	y=no/10;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=1;
	}
	no=no-y*10;
	lcd_data_write(no+'0');
}

// print int8 on LCD
void lcd_print_int8(int8_t no)
{
	int8_t y;
	uint8_t	leading;
	leading=1;
	if(no<0)
	{
		lcd_print_string("-");
		y=no/(-100);
		no=-(no+y*100);
	}
	else
	{
		y=no/100;
		no=no-y*100;
	}

	if(y>0)
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	
	y=no/10;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=1;
	}
	no=no-y*10;
	lcd_data_write(no+'0');
}

// print uint16 on LCD
void lcd_print_uint16(uint16_t no)
{
	uint16_t y;
	uint8_t	leading;
	leading=1;

	y=no/10000;
	if(y>0)
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	no=no-y*10000;

	y=no/1000;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	no=no-y*1000;

	y=no/100;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	no=no-y*100;
	y=no/10;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=1;
	}
	no=no-y*10;
	lcd_data_write(no+'0');
}

// print int16 on LCD
void lcd_print_int16(int16_t no)
{
	int16_t y;
	uint8_t	leading;
	leading=1;

	if(no<0)
	{
		lcd_print_string("-");
		y=no/(-10000);
		no=-(no+y*10000);
	}
	else
	{
		y=no/10000;
		no=no-y*10000;
	}

	if(y>0)
	{
		lcd_data_write(y+'0');
		leading=0;
	}

	y=no/1000;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	no=no-y*1000;

	y=no/100;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=0;
	}
	no=no-y*100;
	y=no/10;
	if((y>0)||(leading==0))
	{
		lcd_data_write(y+'0');
		leading=1;
	}
	no=no-y*10;
	lcd_data_write(no+'0');
}

// prints float on LCD
void lcd_print_float(float no)
{
	uint8_t	x,i;
	int8_t	e;
	float	y;

	if(no<0)
	{
		lcd_print_string("-");
		no=(-no);
	}
	e=log10(no);

	if(e!=0)
	{
		x=no/(pow(10,e));
		y=no/(pow(10,e));
	}
	else
	{
		x=no;
		y=no;
	}
	if(x<1)
	{
		e--;
		x=no/(pow(10,e));
		y=no/(pow(10,e));
	}
	lcd_print_uint8(x);
	lcd_print_string(".");

	for(i=0;i<5;i++)
	{
		y=(y-x)*10;
		x=y;
		lcd_print_uint8(x);
		
	}

	lcd_print_string("E");
	lcd_print_int8(e);

}

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// implements a simple printf for the LCD
// supported formats:
//  %d,%i	signed 16bit integer
//  %c		character
//  %f		float
//  %s		null-terminated string
//	%o		octal number
//  %x		hex number
//  %u		unsigned 16bit integer
//  %%		%
//
// doubles are not supported (floats and doubles are the
//  same for gcc for the AVR
// no formatting is implemented
// \n \t etc not yet supported
//
void lcd_printf(char *fmt, ...)
{
	va_list ap;
	int8_t *p, *sval;
	int8_t cval;
	uint8_t ucval;
	int16_t	ival;
	double dval;
	uint16_t base,uval;
	int8_t scratch[16];

	va_start(ap,fmt);
	for(p=fmt;*p;p++)
	{
		if(*p!='%')
		{
			if(*p=='\n')
			;
			else if(*p=='\t')
			;
			else if(*p=='\b')
			;
			else
				lcd_print_char(*p);
			continue;
		}

		switch(*++p)
		{
			case 'd':
			case 'i':
				ival=va_arg(ap,int16_t);
				lcd_print_int16(ival);
				break;

			case 'c':
				cval=va_arg(ap,int16_t);		// this is not clean does not work with int8_t
				lcd_print_char(cval);			// this seems to be a problem
				break;							// with the va_arg macro

			case 'f':
				dval=va_arg(ap,double);
				lcd_print_float(dval);
				break;

			case 's':
				sval = va_arg(ap,char *);
				lcd_print_string(sval);
				break;

			case 'x':
			case 'X':
				base = 16;
				sval = scratch + 16;
				*--sval = 0;
				uval = va_arg(ap,uint16_t);
				do {
					char ch = uval % base + '0';
					if (ch > '9')
						ch += 'a' - '9' - 1;
					*--sval = ch;
					uval /= base;
				} while (uval);
				lcd_print_string(sval);
				break;

			case 'o':
				base = 8;
				sval = scratch + 16;
				*--sval = 0;
				uval = va_arg(ap,uint16_t);
				do {
					char ch = uval % base + '0';
					if (ch > '9')
					ch += 'a' - '9' - 1;
					*--sval = ch;
					uval /= base;
				} while (uval);
				lcd_print_string(sval);
				break;

			case 'u':
				base = 10;
				sval = scratch + 16;
				*--sval = 0;
				uval = va_arg(ap,uint16_t);
				do {
					char ch = uval % base + '0';
					if (ch > '9')
						ch += 'a' - '9' - 1;
					*--sval = ch;
					uval /= base;
				} while (uval);
				lcd_print_string(sval);
				break;

			default:
				lcd_print_char(*p);
				break;
		}
	}
	va_end(ap);
}


#endif
