/*
 * GccApplication1.c
 *
 * Created: 2019-11-05 오후 9:32:09
 * Author : JY
 */ 
#define F_CPU		16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include "ATmega328_USART.h"

#define nop2			{ asm volatile (" NOP ");	asm volatile(" NOP "); }
#define nop8			{ nop2; nop2; nop2; nop2; }

#define ws2812b_1		PORTB |= (1<<PORTB0)
#define ws2812b_0		PORTB &= ~(1<<PORTB0)

#define MAIN_BTN		(PIND&0x04)
#define LED_BTN			(PIND&0x08)
#define TIMER_BTN		(PIND&0x10)

#define LED_NUM			110
#define GREEN_MAX		255
#define RED_MAX			200
#define GREEN_MIN		5
#define RED_MIN			0

#define MODE_DEFAULT	0

#define TIME_SET_A		5
#define TIME_SET_B		10

volatile unsigned int cnt = 0;
volatile unsigned char main_btn_flag = 0, led_btn_flag = 0, timer_btn_flag = 0;
volatile unsigned char main_btn_cnt = 0, led_btn_cnt = 0, timer_btn_cnt = 0;
volatile unsigned char main_btn_mode = 0, led_btn_mode = 0, timer_btn_mode = 0;
volatile unsigned char dimming_mode = 0, dimming = 255, dimming_cnt = 0;
volatile unsigned char time_cnt = 0, time_sec = 0, time_min = 0;
unsigned char led[LED_NUM][3] = {0, };

// char led_tbl[440] = {
// 	255, 242, 230, 219, 208, 197, 187, 178, 169, 161, 153, 145, 138, 131, 124, 118, 112, 107, 101, 96,
// 	 91,  87,  83,  78,  74,  71,  67,  64,  61,  58,  55,  52,  49,  47,  45,  42,  40,  38,  36, 34,
// 	 33,  31,  30,  28,  27,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,  14,  13, 12, 
// 	 12,  11,  10,   9,   9,   8,   7,   7,   7,   6,   6,   6,   5,   5,   5,   5,   4,   4,   4,  4,
// 	  4,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,  1,
// 	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
// 	//
// 	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,  2,
// 	  2,   2,   3,   3,   3,   3,   3,   3,   3,   4,	4,   4,   4,   4,   5,   5,   5,   5,   6,  6,
// 	  6,   7,   7,   7,   8,   9,   9,  10,  11,  12,  12,  13,  14,  14,  15,  16,  17,  18,  19, 20,
// 	 21,  22,  23,  24,  25,  27,  28,  30,  31,  33,  34,  36,  38,  40,  42,  45,  47,  49,  52, 55,
// 	 58,  61,  64,  67,  71,  74,  78,  83,  87,  91,  96, 101, 107, 112, 118, 124, 131, 138, 145,153,
// 	161, 169, 178, 187, 197, 208, 219, 230, 242, 255,
// 	//
// 	255, 242, 230, 219, 208, 197, 187, 178, 169, 161, 153, 145, 138, 131, 124, 118, 112, 107, 101, 96,
// 	91,  87,  83,  78,  74,  71,  67,  64,  61,  58,  55,  52,  49,  47,  45,  42,  40,  38,  36, 34,
// 	33,  31,  30,  28,  27,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16,  15,  14,  14,  13, 12,
// 	12,  11,  10,   9,   9,   8,   7,   7,   7,   6,   6,   6,   5,   5,   5,   5,   4,   4,   4,  4,
// 	4,   3,   3,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,  1,
// 	1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
// 	//
// 	1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,  2,
// 	2,   2,   3,   3,   3,   3,   3,   3,   3,   4,	4,   4,   4,   4,   5,   5,   5,   5,   6,  6,
// 	6,   7,   7,   7,   8,   9,   9,  10,  11,  12,  12,  13,  14,  14,  15,  16,  17,  18,  19, 20,
// 	21,  22,  23,  24,  25,  27,  28,  30,  31,  33,  34,  36,  38,  40,  42,  45,  47,  49,  52, 55,
// 	58,  61,  64,  67,  71,  74,  78,  83,  87,  91,  96, 101, 107, 112, 118, 124, 131, 138, 145,153,
// 	161, 169, 178, 187, 197, 208, 219, 230, 242, 255,
// };
char led_tbl[440][2] = {
	{255,255 },{242,242 },{230,230 },{219,219 },{208,208 },{197,197 },{187,187 },{178,178 },{169,169 },{161,161 },
	{153,153 },{145,145 },{138,138 },{131,131 },{124,124 },{118,118 },{112,112 },{107,107 },{101,101 },{ 96, 96 },
	{ 91, 91 },{ 87, 87 },{ 83, 83 },{ 78, 78 },{ 74, 74 },{ 71, 71 },{ 67, 67 },{ 64, 64 },{ 61, 61 },{ 58, 58 },
	{ 55, 55 },{ 52, 52 },{ 49, 49 },{ 47, 47 },{ 45, 45 },{ 42, 42 },{ 40, 40 },{ 38, 38 },{ 36, 36 },{ 34, 34 },
	{ 33, 33 },{ 31, 31 },{ 30, 30 },{ 28, 28 },{ 27, 27 },{ 25, 25 },{ 24, 24 },{ 23, 23 },{ 22, 22 },{ 21, 21 },
	{ 20, 20 },{ 19, 19 },{ 18, 18 },{ 17, 17 },{ 16, 16 },{ 15, 15 },{ 14, 14 },{ 14, 14 },{ 13, 13 },{ 12, 12 },
	{ 12, 12 },{ 11, 11 },{ 10, 10 },{  9,  9 },{  9,  9 },{  8,  8 },{  7,  7 },{  7,  7 },{  7,  7 },{  6,  6 },
	{  6,  6 },{  6,  6 },{  5,  5 },{  5,  5 },{  5,  5 },{  5,  5 },{  4,  4 },{  4,  4 },{  4,  4 },{  4,  4 },
	{  4,  4 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  2,  2 },{  2,  2 },
	{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  1,  1 },{  1,  1 },
	{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },
	//
	{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },
	{  1,  1 },{  1,  1 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },
	{  2,  2 },{  2,  2 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  4,  4 },
	{  4,  4 },{  4,  4 },{  4,  4 },{  4,  4 },{  5,  5 },{  5,  5 },{  5,  5 },{  5,  5 },{  6,  6 },{  6,  6 },
	{  6,  6 },{  7,  7 },{  7,  7 },{  7,  7 },{  8,  8 },{  9,  9 },{  9,  9 },{ 10, 10 },{ 11, 11 },{ 12, 12 },
	{ 12, 12 },{ 13, 13 },{ 14, 14 },{ 14, 14 },{ 15, 15 },{ 16, 16 },{ 17, 17 },{ 18, 18 },{ 19, 19 },{ 20, 20 },
	{ 21, 21 },{ 22, 22 },{ 23, 23 },{ 24, 24 },{ 25, 25 },{ 27, 27 },{ 28, 28 },{ 30, 30 },{ 31, 31 },{ 33, 33 },
	{ 34, 34 },{ 36, 36 },{ 38, 38 },{ 40, 40 },{ 42, 42 },{ 45, 45 },{ 47, 47 },{ 49, 49 },{ 52, 52 },{ 55, 55 },
	{ 58, 58 },{ 61, 61 },{ 64, 64 },{ 67, 67 },{ 71, 71 },{ 74, 74 },{ 78, 78 },{ 83, 83 },{ 87, 87 },{ 91, 91 },
	{ 96, 96 },{101,101 },{107,107 },{112,112 },{118,118 },{124,124 },{131,131 },{138,138 },{145,145 },{153,153 },
	{161,161 },{169,169 },{178,178 },{187,187 },{197,197 },{208,208 },{219,219 },{230,230 },{242,242 },{255,255 },
	//
	{255,255 },{242,242 },{230,230 },{219,219 },{208,208 },{197,197 },{187,187 },{178,178 },{169,169 },{161,161 },
	{153,153 },{145,145 },{138,138 },{131,131 },{124,124 },{118,118 },{112,112 },{107,107 },{101,101 },{ 96, 96 },
	{ 91, 91 },{ 87, 87 },{ 83, 83 },{ 78, 78 },{ 74, 74 },{ 71, 71 },{ 67, 67 },{ 64, 64 },{ 61, 61 },{ 58, 58 },
	{ 55, 55 },{ 52, 52 },{ 49, 49 },{ 47, 47 },{ 45, 45 },{ 42, 42 },{ 40, 40 },{ 38, 38 },{ 36, 36 },{ 34, 34 },
	{ 33, 33 },{ 31, 31 },{ 30, 30 },{ 28, 28 },{ 27, 27 },{ 25, 25 },{ 24, 24 },{ 23, 23 },{ 22, 22 },{ 21, 21 },
	{ 20, 20 },{ 19, 19 },{ 18, 18 },{ 17, 17 },{ 16, 16 },{ 15, 15 },{ 14, 14 },{ 14, 14 },{ 13, 13 },{ 12, 12 },
	{ 12, 12 },{ 11, 11 },{ 10, 10 },{  9,  9 },{  9,  9 },{  8,  8 },{  7,  7 },{  7,  7 },{  7,  7 },{  6,  6 },
	{  6,  6 },{  6,  6 },{  5,  5 },{  5,  5 },{  5,  5 },{  5,  5 },{  4,  4 },{  4,  4 },{  4,  4 },{  4,  4 },
	{  4,  4 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  2,  2 },{  2,  2 },
	{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  1,  1 },{  1,  1 },
	{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },
	//
	{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },{  1,  1 },
	{  1,  1 },{  1,  1 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },{  2,  2 },
	{  2,  2 },{  2,  2 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  3,  3 },{  4,  4 },
	{  4,  4 },{  4,  4 },{  4,  4 },{  4,  4 },{  5,  5 },{  5,  5 },{  5,  5 },{  5,  5 },{  6,  6 },{  6,  6 },
	{  6,  6 },{  7,  7 },{  7,  7 },{  7,  7 },{  8,  8 },{  9,  9 },{  9,  9 },{ 10, 10 },{ 11, 11 },{ 12, 12 },
	{ 12, 12 },{ 13, 13 },{ 14, 14 },{ 14, 14 },{ 15, 15 },{ 16, 16 },{ 17, 17 },{ 18, 18 },{ 19, 19 },{ 20, 20 },
	{ 21, 21 },{ 22, 22 },{ 23, 23 },{ 24, 24 },{ 25, 25 },{ 27, 27 },{ 28, 28 },{ 30, 30 },{ 31, 31 },{ 33, 33 },
	{ 34, 34 },{ 36, 36 },{ 38, 38 },{ 40, 40 },{ 42, 42 },{ 45, 45 },{ 47, 47 },{ 49, 49 },{ 52, 52 },{ 55, 55 },
	{ 58, 58 },{ 61, 61 },{ 64, 64 },{ 67, 67 },{ 71, 71 },{ 74, 74 },{ 78, 78 },{ 83, 83 },{ 87, 87 },{ 91, 91 },
	{ 96, 96 },{101,101 },{107,107 },{112,112 },{118,118 },{124,124 },{131,131 },{138,138 },{145,145 },{153,153 },
	{161,161 },{169,169 },{178,178 },{187,187 },{197,197 },{208,208 },{219,219 },{230,230 },{242,242 },{255,255 }
};

char temp[40];

ISR(TIMER0_COMPA_vect)
{
	time_cnt++;
	if(time_cnt%20 == 0)		PORTB ^= (1 << PORTB5);
	if(time_cnt == 100)
	{
		time_sec++;
		time_cnt = 0;
	}
	if(time_sec == 60)
	{
		time_min++;
		time_sec = 0;
	}
	
	if(main_btn_mode == 1)
	{
		if(timer_btn_mode == 1)
		{
			if(time_min == TIME_SET_A)
			{
				sprintf(temp, "--- main_mode  :  %dmin OFF  ---\n", time_min);
				USART_Transmit_str(temp, _ASCII);
				main_btn_mode  = MODE_DEFAULT;
				led_btn_mode   = MODE_DEFAULT;
				timer_btn_mode = MODE_DEFAULT;
			}
		}
		else if(timer_btn_mode == 2)
		{
			if(time_min == TIME_SET_B)
			{
				sprintf(temp, "--- main_mode  :  %dmin OFF  ---\n", time_min);
				USART_Transmit_str(temp, _ASCII);
				main_btn_mode  = MODE_DEFAULT;
				led_btn_mode   = MODE_DEFAULT;
				timer_btn_mode = MODE_DEFAULT;
			}
		}
	}
	
	if(main_btn_mode == 0 && led_btn_mode == 0 && timer_btn_mode == 0)	PORTB &= ~(1 << PORTB5);
}

ISR(TIMER1_COMPA_vect)
{
	// ************************** MAIN_BTN PROCESS ************************** //
// 	sprintf(temp, "--- Now dimming = %d\n", dimming);
// 	USART_Transmit_str(temp, _ASCII);
	
	if(!MAIN_BTN)
	{
		if(main_btn_flag == 0)
		{
			main_btn_flag = 1;
			main_btn_cnt = 0;
		}
		else
		{
			if(main_btn_cnt < 60)	main_btn_cnt++;
		}
	}
	else
	{
		main_btn_flag = 0;
		if(main_btn_cnt)
		{
			if(main_btn_cnt >= 20)		// main_btn 길게 누름
			{
				// Bluetooth ON
				if(main_btn_mode == 1)
				{
					sprintf(temp, "--- main_btn_mode  :  Bluetooth ON ---\n");
					USART_Transmit_str(temp, _ASCII);
				}
			}
			else								// main_btn 짧게 누름
			{
				if(main_btn_mode == 0)
				{
					main_btn_mode = 1;			// 전원 ON
					led_btn_mode = 1;			// led mode = ON
					dimming = 255;
					sprintf(temp, "--- main_mode  :  ON  ---\n");
				}
				else 
				{
					main_btn_mode  = MODE_DEFAULT;
					led_btn_mode   = MODE_DEFAULT;
					timer_btn_mode = MODE_DEFAULT;
					sprintf(temp, "--- main_mode  :  OFF ---\n");
				}
				USART_Transmit_str(temp, _ASCII);
			}
		}
		main_btn_cnt = 0;
	}
	
	// ************************** LED_BTN PROCESS ************************** //
	if(!LED_BTN)
	{
		if(led_btn_flag == 0)
		{
			led_btn_flag = 1;
			led_btn_cnt = 0;
			dimming_cnt = 0;
		}
		else
		{
			led_btn_cnt++;
			if(led_btn_mode == 1 || led_btn_mode == 2)
			{
				if(dimming_mode == 0)
				{
					if(dimming > 0)			dimming = dimming - 5;
				}
				else
				{
					if(dimming < 255)		dimming = dimming + 5;
				}
			}
		}
	}
	else
	{
		led_btn_flag = 0;
		if(led_btn_cnt)
		{
			if(led_btn_cnt > 10)
			{
				if(led_btn_mode == 1 || led_btn_mode == 2)
				{
					dimming_mode = !dimming_mode;
				}
				if(led_btn_mode == 1)		sprintf(temp, "--- led_mode  :  ON  ---,  dimming = %d\n", dimming);
				else if(led_btn_mode == 2)	sprintf(temp, "--- led_mode  :  ANI ---,  dimming = %d\n", dimming);
				USART_Transmit_str(temp, _ASCII);
			}
			else
			{
				if(main_btn_mode == 1)
				{
					// led_btn_mode = 0	------------- LED OFF
					// led_btn_mode = 1	------------- LED ON
					// led_btn_mode = 2	------------- Animation mode
					led_btn_mode++;
					if(led_btn_mode > 2)	led_btn_mode = 0;
					if(led_btn_mode == 0)		sprintf(temp, "--- led_mode  :  OFF ---,  dimming = %d\n", dimming);
					else if(led_btn_mode == 1)	sprintf(temp, "--- led_mode  :  ON  ---,  dimming = %d\n", dimming);
					else						sprintf(temp, "--- led_mode  :  ANI ---,  dimming = %d\n", dimming);
					USART_Transmit_str(temp, _ASCII);
				}
			}
		}
		led_btn_cnt = 0;
		dimming_cnt = 0;
	}
	
	// ************************** TIMER_BTN PROCESS ************************** //
	if(!TIMER_BTN)
	{
		if(timer_btn_flag == 0)
		{
			timer_btn_flag = 1;
			timer_btn_cnt = 0;
		}
		else
		{
			if(timer_btn_cnt < 60)	timer_btn_cnt++;
		}
	}
	else
	{
		timer_btn_flag = 0;
		if(timer_btn_cnt)
		{
			if(main_btn_mode == 1)
			{
				// timer_btn_mode = 1	------------- Turn off after timer 30min
				// timer_btn_mode = 2	------------- Turn off after timer 60min
				// timer_btn_mode = 0	------------- Turn on and timer off
				time_cnt = 0;	time_sec = 0;	time_min = 0;
				timer_btn_mode++;
				if(timer_btn_mode > 2)	timer_btn_mode = 0;
				if(timer_btn_mode == 0)			sprintf(temp, "--- timer_mode  = Timer off     ---\n");
				else if(timer_btn_mode == 1)	sprintf(temp, "--- timer_mode  = Timer 30min   ---\n");
				else							sprintf(temp, "--- timer_mode  = Timer 60min   ---\n");
				USART_Transmit_str(temp, _ASCII);
			}
		}
		timer_btn_cnt = 0;
	}
}


void byte_out(char d)
{
	char i;
	for(i=0;i<8;i++)
	{
		if(d&0x80)
		{ 
			ws2812b_1; 
			_delay_loop_1( 2); 
			ws2812b_0; 
			_delay_loop_1( 1); 
		}
		else 
		{ 
			ws2812b_1; 
			_delay_loop_1( 1); 
			ws2812b_0; 
			_delay_loop_1( 2); 
		}
		d<<=1;
	}
}

void led_control(char num)
{
	int i, j;
	if(num == 2)
	{
		for(j=0;j<LED_NUM*2;j++)
		{
			for(i=0;i<LED_NUM;i++)
			{
				led[i][0] = led_tbl[i+j][0];
				led[i][1] = led_tbl[i+j][1];
// 				led[i][0] = led_tbl[i+j];
// 				led[i][1] = led_tbl[i+j];
				led[i][2] = 0;
			}
			for(i=0;i<LED_NUM;i++)
			{
				byte_out(led[i][1]);
				byte_out(led[i][0]);
				byte_out(0);
			}
			_delay_ms(30);
			if(led_btn_mode != 2)	break;
		}
	}
}

void led_display(void)
{
	for(int i=0;i<LED_NUM;i++)
	{
		led[i][0] = dimming;
		led[i][1] = dimming;
		led[i][2] = 0; // clear
	}
	for(int i=0;i<LED_NUM;i++)
	{
		byte_out(led[i][1]);
		byte_out(led[i][0]);
		byte_out(led[i][2]);
	}
	_delay_us(300);
}

void led_off(void)
{
	for(int i=0;i<LED_NUM;i++)
	{
		led[i][0] = 0;
		led[i][1] = 0;
		led[i][2] = 0; // clear
	}
	for(int i=0;i<LED_NUM;i++)
	{
		byte_out(led[i][1]);
		byte_out(led[i][0]);
		byte_out(led[i][2]);
	}	
	_delay_ms(20);
}


int main(void)
{
    DDRB = (1 << PORTB0) | (1 << PORTB5);		// PORTB0 = WS2812B, PORTB5 = status debug
	TCCR0A = (1 << WGM01);			// CTC Mode
	TCCR0B = (1 << CS00) | (1 << CS02);		// 1024 Prescale
	TIMSK0 = (1 << OCIE0A);			// Compare A Match Interrupt Enable
	OCR0A = 0x9C;					// 10ms TIMER SET
	
	TCCR1A = 0x00;                  // CTC Mode
	TCCR1B = 0x0B;                  // 64 prescale        16000000 / 64 65535
	TIMSK1 = 0x02;                  // Compare A Match Interrupt Enable
	OCR1A = 0x30D4;                 // 50ms TIMER SET
	USART_Init(BAUD_9600);
	led_off();
	
	_delay_ms(500);
	sprintf(temp, "--- Now Start ---\n");
	USART_Transmit_str(temp, _ASCII);
	sei();
    while (1) 
    {
		led_display();
// 		if(main_btn_mode == 1 && led_btn_mode == 1)
// 		{
// 			led_display();
// 		}
// 		else if(main_btn_mode == 1 && led_btn_mode == 2)
// 		{
// 	 		led_control(2);
// 		}
// 		else if(main_btn_mode == 0 || led_btn_mode == 0)
// 		{
// 			PORTB &= ~(1 << PORTB5);
// 			led_off();
// 		}
    }
}

