/*
 * ATmega328_USART.h
 *
 * Created: 2018-12-12 오전 9:25:03
 *  Author: Jae
 */ 


#ifndef ATMEGA328_USART_H_
#define ATMEGA328_USART_H_

#include "common.h"

#define BAUD_9600			103
#define BAUD_19200			51
#define BAUD_38400			25
#define BAUD_115200			8
#define BAUD_1000000		0

#define _ASCII				10
#define _HEX				11

#define LENTH				3

extern uint8_t rx_data, rx_buf[50], rx_cnt;
extern volatile uint8_t str[50], rx_flag;

void USART_Init(int ubrr);
void USART_Transmit_str(char* data, uint8_t type);
void USART_Transmit_char(uint8_t data);
void USART_Transmit_charBytes(uint8_t* data, int Length);
void translateChartoASCII(uint8_t data);
uint8_t nibbleToHexChar(uint8_t data);
void USART_Transmit_byteTostr(uint8_t data);

uint8_t USART_Receive_char(void);
void USART_Receive_str(uint8_t myString[], uint8_t maxLength);

// void USART_Transmit(char data, FILE *stream);
// char USART_Receive(FILE *stream);

/*FILE uart_stream = FDEV_SETUP_STREAM(USART_Transmit, USART_Receive, _FDEV_SETUP_RW);*/


#endif /* ATMEGA328_USART_H_ */