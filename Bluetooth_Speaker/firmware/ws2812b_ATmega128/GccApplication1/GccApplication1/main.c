/*
 * GccApplication1.c
 *
 * Created: 19-11-11 월 오전 10:05:38
 * Author : LABC
 */ 

#define F_CPU		16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "128_GPIO.h"
#include "128_ADC.h"
#include "128_UART.h"

char temp[100];

void display_FND(int value)
{
	char digit[12] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67, 0x00, 0x40};
	char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
	int num[4], i;
	
	if(value < 0)
	{
		num[3] = 11;				// 음수인 경우는 ‘-’ 디스플레이
		value = (~value)+1;			// 2’s Compliment (절대값)
	}
	else
	{
		num[3] = value / 1000;
	}
	num[2] = (value / 100) % 10;
	num[1] = (value / 10) % 10;
	num[0] = value % 10;
	
	for(i=0; i<4; i++)
	{
		PORTC = digit[num[i]];
		PORTG = fnd_sel[i];
 		PORTC |= 0x80;
		_delay_ms(2);
	}
}


int main(void)
{
	int data;
	for(int i=0; i<8; i++)		j_pinMode('C', i, OUTPUT);
	j_pinMode('G', 0, OUTPUT);
	j_pinMode('G', 1, OUTPUT);
	j_pinMode('G', 2, OUTPUT);
	j_pinMode('G', 3, OUTPUT);
	init_uart0();
	sei();
    while (1) 
    {
		for(int i=0; i<30; i++)
		{
			if(i == 0)		data = j_analogRead(0);
			display_FND(data);
		}
		
    }
}


