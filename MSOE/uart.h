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
	uart.h
	Header file for the uart interface
	Copyright (c) 2004 Joerg Mossbrucker <mossbruc@msoe.edu>
*/

#ifndef _UART_H
#define _UART_H

#include "globaldef.h"
//#include "uartconf.h"

// set rx buffer size
#define BUFFERSIZE 128
// set baud rate to 9600
#define UART_BAUD_RATE      9600
//set buad rate register to right value
//works only for baud rates>=4800

#define UART_BAUD_SELECT (F_CPU/(UART_BAUD_RATE*16l)-1)

/* uart functions */
uint8_t uart_receive_byte(void);
void uart_send_byte(uint8_t tx_char);
void uart_send(uint8_t *buf, uint8_t no_of_bytes);
void uart_init(void);

#endif
