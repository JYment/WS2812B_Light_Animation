/*
 * ATmega328_USART.c
 *
 * Created: 2018-12-12 오전 9:26:18
 *  Author: Jae
 */ 
#include "ATmega328_USART.h"

uint8_t rx_data, rx_buf[50], rx_cnt;
volatile uint8_t str[50], rx_flag = 0;

void USART_Init(int ubrr)
{
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);			// RX, TX interrupt Enable, RX, TX enable
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

// 문자 출력
void USART_Transmit_char(uint8_t data)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;	
}

// 지정한 length 만큼 문자 출력
void USART_Transmit_charBytes(uint8_t* data, int Length)
{
	int i=0;
	for(i=0; i<Length; i++)
	{
		USART_Transmit_char(*(data+i));
	}
}

// 문자열 출력
void USART_Transmit_str(char* data, uint8_t type)
{
	if(type == _ASCII)
	{
		while(*data)
		{
			USART_Transmit_char(*data++);
		}
	}
	else if(type == _HEX)
	{
		while(*data)
		{
			translateChartoASCII(*data++);
		}
	}
}


uint8_t nibbleToHexChar(uint8_t data)				// Converts 4 bits into hexadecimal
{
	if(data < 10)
	{
		return ('0' + data);
	}
	else
	{
		return ('A' + data - 10);
	}
}

// char를 ASCII 문자형으로 변경
void translateChartoASCII(uint8_t data)
{
	uint8_t nibble[2];

	nibble[0] = (data & 0b11110000) >> 4;
	nibble[1] = data & 0b00001111;	
	nibble[0] = nibbleToHexChar(nibble[0]);
	nibble[1] = nibbleToHexChar(nibble[1]);
	USART_Transmit_char(nibble[0]);	
	USART_Transmit_char(nibble[1]);
}

// 한문자 입력
uint8_t USART_Receive_char(void)
{
	while(!(UCSR0A & (1 << RXC0)));

	return UDR0;
}

// 문자열 입력
void USART_Receive_str(uint8_t myString[], uint8_t maxLength)
{
	uint8_t response, i = 0;
	while(i < (maxLength))
	{	
		response = USART_Receive_char();
		if(response == '\r')
		{
			break;
		}
		else
		{
			myString[i] = response;
			i++;
		}
	}
	myString[i] = 0;
}

// unsigned char USART_Receive(FILE *stream)
// {
// 	while(!(UCSR0A & (1 << RXC0)));
// 	return UDR0;
// }
// 
// void USART_Transmit(char data, FILE *stream)
// {
// 	while(!(UCSR0A & (1 << UDRE0)));
// 	UDR0 = data;
// }

ISR(USART_RX_vect)
{
	rx_data = UDR0;
	rx_buf[rx_cnt++] = rx_data;
	USART_Transmit_char(rx_data);
	if(rx_cnt == LENTH)
	{
		memcpy(str, rx_buf, LENTH);
		str[LENTH] = 0;
		rx_cnt = 0;
		rx_flag = 1;
	}
}