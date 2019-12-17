/*
 * _128_UART.h
 *
 * Created: 19-11-11 월 오후 03:56:53
 *  Author: LABC
 */ 

#include <stdio.h>

#define STX		0x5F
#define ETX		0x2F


char rx_buf[20], rx_cnt = 0;
char tx_buf[20], tx_cnt = 0;
volatile char rx_flag = 0;

void init_uart0()
{
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0H = 0;
	UBRR0L = 103;		//9600
}

void tx0_char(char c)
{
	while(!(UCSR0A & 0x20));
	UDR0 = c;
}

void tx0_str(char *ptr)
{
	while(1)
	{
		if(*ptr != NULL)	tx0_char(*ptr++);
		else                return;
	}
}

char rx0_char(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

ISR(USART0_RX_vect)
{
	int i;
	char rxdata;
	rxdata = UDR0;
	if(!rx_flag)
	{
		if(rxdata == STX)
		{
			rx_cnt = 0;
		}
		else if(rxdata == ETX)
		{
			for(i=0; (i<rx_cnt) && (i < 6); i++)
			{
				tx_buf[i] = rx_buf[i];
				rx_buf[i] = ' ';
			}
			rx_cnt = 0;
			rx_flag = 1;
		}
		else
		{
			if(rx_cnt < 10)
			{
				rx_buf[rx_cnt] = rxdata;
				rx_cnt++;
			}
		}
	}
}