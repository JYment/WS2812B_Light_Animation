/*
 * ws2812b.c
 *
 * Created: 2019-11-04 오후 9:54:44
 * Author : JY
 */ 

#define F_CPU		16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define nop2	{asm volatile(" NOP ");	asm volatile(" NOP ");}
#define nop8	{nop2; nop2; nop2; nop2;}
#define WS2812b_HIGH	PORTB |= (1 << PB3)
#define WS2812b_LOW		PORTB &= ~(1 << PB3)


#define LED_NUM			100
#define GREEN_MAX			255
#define RED_MAX				200
#define GREEN_MIN			55
#define RED_MIN				0


char led[26][3]={0,};
char led_tbl[156][3]={
	{255,  0,  0},{245,  0, 10},{235,  0, 20},{224,  0, 31},{214,  0, 41},
	{204,  0, 51},{194,  0, 61},{184,  0, 71},{173,  0, 82},{163,  0, 92},
	{153,  0,102},{143,  0,112},{133,  0,122},{122,  0,133},{112,  0,143},
	{102,  0,153},{ 92,  0,163},{ 82,  0,173},{ 71,  0,184},{ 61,  0,194},
	{ 51,  0,204},{ 41,  0,214},{ 31,  0,224},{ 20,  0,235},{ 10,  0,245},
	{  0,  0,255},{  0,  0,255},{  0, 10,245},{  0, 20,235},{  0, 31,224},
	{  0, 41,214},{  0, 51,204},{  0, 61,194},{  0, 71,184},{  0, 82,173},
	{  0, 92,163},{  0,102,153},{  0,112,143},{  0,122,133},{  0,133,122},
	{  0,143,112},{  0,153,102},{  0,163, 92},{  0,173, 82},{  0,184, 71},
	{  0,194, 61},{  0,204, 51},{  0,214, 41},{  0,224, 31},{  0,235, 20},
	{  0,245, 10},{  0,255,  0},{  0,255,  0},{ 10,245,  0},{ 20,235,  0},
	{ 31,224,  0},{ 41,214,  0},{ 51,204,  0},{ 61,194,  0},{ 71,184,  0},
	{ 82,173,  0},{ 92,163,  0},{102,153,  0},{112,143,  0},{122,133,  0},
	{133,122,  0},{143,112,  0},{153,102,  0},{163, 92,  0},{173, 82,  0},
	{184, 71,  0},{194, 61,  0},{204, 51,  0},{214, 41,  0},{224, 31,  0},
	{235, 20,  0},{245, 10,  0},{255,  0,  0},{255,  0,  0},{245,  0, 10},
	{235,  0, 20},{224,  0, 31},{214,  0, 41},{204,  0, 51},{194,  0, 61},
	{184,  0, 71},{173,  0, 82},{163,  0, 92},{153,  0,102},{143,  0,112},
	{133,  0,122},{122,  0,133},{112,  0,143},{102,  0,153},{ 92,  0,163},
	{ 82,  0,173},{ 71,  0,184},{ 61,  0,194},{ 51,  0,204},{ 41,  0,214},
	{ 31,  0,224},{ 20,  0,235},{ 10,  0,245},{  0,  0,255},{  0,  0,255},
	{  0, 10,245},{  0, 20,235},{  0, 31,224},{  0, 41,214},{  0, 51,204},
	{  0, 61,194},{  0, 71,184},{  0, 82,173},{  0, 92,163},{  0,102,153},
	{  0,112,143},{  0,122,133},{  0,133,122},{  0,143,112},{  0,153,102},
	{  0,163, 92},{  0,173, 82},{  0,184, 71},{  0,194, 61},{  0,204, 51},
	{  0,214, 41},{  0,224, 31},{  0,235, 20},{  0,245, 10},{  0,255,  0},
	{  0,255,  0},{ 10,245,  0},{ 20,235,  0},{ 31,224,  0},{ 41,214,  0},
	{ 51,204,  0},{ 61,194,  0},{ 71,184,  0},{ 82,173,  0},{ 92,163,  0},
	{102,153,  0},{112,143,  0},{122,133,  0},{133,122,  0},{143,112,  0},
	{153,102,  0},{163, 92,  0},{173, 82,  0},{184, 71,  0},{194, 61,  0},
	{204, 51,  0},{214, 41,  0},{224, 31,  0},{235, 20,  0},{245, 10,  0},{255,  0,  0}
};


char tbl[120][3];
char temp[20];


void tx0_char(char c)
{
	while(!(UCSRA & 0x20));
	UDR = c;
}


void tx0_str(char *ptr)
{
	while(1)
	{
		if(*ptr != NULL)
		tx0_char(*ptr++);
		else
		return;
	}
}



void uart_init(void)
{
	UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
	UBRRH = 0;
	UBRRL = 103;		//9600
}


void byte_out(unsigned char d)
{
	unsigned char i;
	for(i=0; i<8; i++)
	{
		if(d & 0x80)
		{
			WS2812b_HIGH;
			nop8;
			WS2812b_LOW;
		}
		else
		{
			WS2812b_HIGH;
			nop2;
			WS2812b_LOW;
		}
		d <<= 1;
	}
}


void led_control(char num)
{
	int i = 0, j = 0;
// 	sprintf(temp, "num = %d\n", num);
// 	tx0_str(temp);
	if(num == 0)
	{
		for(i=0; i<120; i++)
		{
			led_tbl[i][0] = 0;
			led_tbl[i][1] = 0;
			led_tbl[i][2] = 0;			
		}
		
		for(i=0; i<LED_NUM/2; i++)
		{
			led_tbl[i][0] = GREEN_MAX - j;
			led_tbl[i][1] = RED_MAX - j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		j = 0;
		
		for(i=LED_NUM/2; i<LED_NUM; i++)
		{
			led_tbl[i][0] = GREEN_MIN + j;
			led_tbl[i][1] = RED_MIN + j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		
		// 		for(i=0; i<LED_NUM; i++)
		// 		{
		// 			byte_out(led_tbl[i][0]);			// G
		// 			byte_out(led_tbl[i][1]);			// R
		// 			byte_out(led_tbl[i][2]);			// B
		// 			sprintf(temp, " %d = G:%d   R:%d\n", i, led_tbl[i][0], led_tbl[i][1]);
		// 			tx0_str(temp);
		// 			_delay_ms(20);
		// 		}
	}
	else if(num == 1)
	{
		for(i=LED_NUM/2; i>=0; i--)
		{
			led_tbl[i][0] = GREEN_MAX - j;
			led_tbl[i][1] = RED_MAX - j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		j = 0;
		
		for(i=LED_NUM-1; i>=LED_NUM/2; i--)
		{
			led_tbl[i][0] = GREEN_MIN + j;
			led_tbl[i][1] = RED_MIN + j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		
		// 		for(i=0; i<LED_NUM; i++)
		// 		{
		// 			byte_out(led_tbl[i][0]);			// G
		// 			byte_out(led_tbl[i][1]);			// R
		// 			byte_out(led_tbl[i][2]);			// B
		// 			sprintf(temp, "%d  =  G:%d   R:%d\n", i, led_tbl[i][0], led_tbl[i][1]);
		// 			tx0_str(temp);
		// 			_delay_ms(20);
		// 		}
	}
	else if(num == 2)
	{
		for(i=0; i<LED_NUM/2; i++)
		{
			led_tbl[i][0] = GREEN_MIN + j;
			led_tbl[i][1] = RED_MIN + j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		j = 0;
		
		for(i=LED_NUM/2; i<LED_NUM; i++)
		{
			led_tbl[i][0] = GREEN_MIN + j;
			led_tbl[i][1] = RED_MIN + j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		
// 		for(i=0; i<LED_NUM; i++)
// 		{
// 			byte_out(led_tbl[i][0]);			// G
// 			byte_out(led_tbl[i][1]);			// R
// 			byte_out(led_tbl[i][2]);			// B
// 			sprintf(temp, "%d  =  G:%d   R:%d\n", i, led_tbl[i][0], led_tbl[i][1]);
// 			tx0_str(temp);
// 			_delay_ms(20);
// 		}
	}
	else
	{
		for(i=LED_NUM/2; i>=0; i--)
		{
			led_tbl[i][0] = GREEN_MAX - j;
			led_tbl[i][1] = RED_MAX - j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}
		j = 0;
		
		for(i=LED_NUM-1; i>=LED_NUM/2; i--)
		{
			led_tbl[i][0] = GREEN_MAX - j;
			led_tbl[i][1] = GREEN_MAX - j;
			led_tbl[i][2] = 1;
			j = j + 4;
		}

// 		for(i=0; i<LED_NUM; i++)
// 		{
// 			byte_out(led_tbl[i][0]);			// G
// 			byte_out(led_tbl[i][1]);			// R
// 			byte_out(led_tbl[i][2]);			// B
// 			sprintf(temp, "%d  =  G:%d   R:%d\n", i, led_tbl[i][0], led_tbl[i][1]);
// 			tx0_str(temp);
// 			_delay_ms(20);
// 		}
	}
}


void led_show(void)
{
	int i;
	for(i=0; i<10; i++)
	{
		byte_out(led_tbl[i][0]);			// G
		byte_out(led_tbl[i][1]);			// R
		byte_out(led_tbl[i][2]);			// B
// 		sprintf(temp, "%d  =  G:%d   R:%d\n", i, led_tbl[i][0], led_tbl[i][1]);
// 		tx0_str(temp);
	}
	_delay_ms(1);
}



int main(void)
{
	int i, shift;
	DDRB = (1 << PB3);
    while (1) 
    {
		 for(shift=0; shift<78; shift++)
		 {
			 for(i=0;i<26;i++){ led[i][0]=led_tbl[i+shift][0]; led[i][1]=led_tbl[i+shift][1]; led[i][2]=led_tbl[i+shift][2]; }
			 for(i=0;i<26;i++){ byte_out(led[i][1]);           byte_out(led[i][0]);           byte_out(led[i][2]);           } //GRB
			 _delay_ms(50);
		 }
		 
		
// 		for(j=0; j<100; j++)
// 		{
// 			led_tbl[j][0] = 255;
// 			led_tbl[j][1] = 200;
// 			led_tbl[j][2] = 1;
// 		}
// 		for(i=0; i<LED_NUM; i++)
// 		{
// 			byte_out(led_tbl[i][0]);			// G
// 			byte_out(led_tbl[i][1]);			// R
// 			byte_out(led_tbl[i][2]);			// B
// 		}
// 		_delay_us(1);
	}
}
