/* ********************************************************
   * FILENAME: lab1.c                                     *
   * AUTHOR: Dan Kass kassd@msoe.edu                      *
   * DATE: 11/29/12                                       *
   * Course: CE-2811                                      *
   *                                                      *
   * This lab is the knight rider lab where the goal is   *
   *     to have the lights on the microcontroler run     *
   *     back and forth untill the device is turned off   *
   ******************************************************** */
/* 
            ***Some issues with the lab***
I had a some problems getting my board to with the computer. 
	Some driver settings were not correct, once that was fixed
	the board worked correctly.
The other problem I had is I forgot to set the Data Direction 
	Register the first time I tried to run the program. As soon
	as I set that it worked perfectly. 
*/
	
#include <avr/io.h>
#include <stdlib.h>
#include "MSOE/delay.c"

#define DELAY 25  //sets delay to 25 milisenconds

int main(void)
{
	DDRB=0xFF; //Sets the Data Direction Register
	while(1)
	{
		//outputs to PORT B to light up one led then
		//waits 10 miliseconds and outputs the led next to it
		PORTB = 0b10000000;
		delay_ms(DELAY);
		PORTB = 0b01000000;
		delay_ms(DELAY);
		PORTB = 0b00100000;
		delay_ms(DELAY);
		PORTB = 0b00010000;
		delay_ms(DELAY);
		PORTB = 0b00001000;
		delay_ms(DELAY);
		PORTB = 0b00000100;
		delay_ms(DELAY);
		PORTB = 0b00000010;
		delay_ms(DELAY);
		PORTB = 0b00000001;
		delay_ms(DELAY);
		PORTB = 0b00000010;
		delay_ms(DELAY);
		PORTB = 0b00000100;
		delay_ms(DELAY);
		PORTB = 0b00001000;
		delay_ms(DELAY);
		PORTB = 0b00010000;
		delay_ms(DELAY);
		PORTB = 0b00100000;
		delay_ms(DELAY);
		PORTB = 0b01000000;
		delay_ms(DELAY);
		PORTB = 0b10000000;
		delay_ms(DELAY);
	
	} //while
} //main



