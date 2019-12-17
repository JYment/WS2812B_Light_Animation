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
#include "light_ws2812.h"

#define MAIN_BTN		(PIND&0x04)
#define LED_BTN			(PIND&0x08)
#define TIMER_BTN		(PIND&0x10)

#define LED_NUM			80

#define MODE_DEFAULT	0

#define TIME_SET_A		30
#define TIME_SET_B		60


volatile unsigned int cnt = 0;
volatile unsigned char main_btn_flag = 0, led_btn_flag = 0, timer_btn_flag = 0;
volatile unsigned char main_btn_cnt = 0, led_btn_cnt = 0, timer_btn_cnt = 0;
volatile unsigned char main_btn_mode = 0, led_btn_mode = 0, timer_btn_mode = 0;
volatile unsigned char dimming_mode = 0, dimming = 1, dimming_cnt = 0;
volatile unsigned char time_cnt = 0, time_sec = 0, time_min = 0;
struct cRGB led[80];
unsigned char ani_state = 0;

// char led_tbl[440][2] = {
// 	{2,1},{5,3},{7,4},{9,5},{12,7},{14,8},{16,10},{19,11},{21,12},{23,14},
// 	{26,15},{28,16},{30,18},{32,19},{35,20},{37,22},{39,23},{42,25},{44,26},{46,27},
// 	{49,29},{51,30},{53,31},{56,33},{58,34},{60,35},{63,37},{65,38},{67,40},{70,41},
// 	{72,42},{74,44},{77,45},{79,46},{81,48},{83,49},{86,50},{88,52},{90,53},{93,55},
// 	{95,56},{97,57},{100,59},{102,60},{104,61},{107,63},{109,64},{111,65},{114,67},{116,68},
// 	{118,70},{121,71},{123,72},{125,74},{128,75},{130,76},{132,78},{134,79},{137,80},{139,82},
// 	{141,83},{144,85},{146,86},{148,87},{151,89},{153,90},{155,91},{158,93},{160,94},{162,95},
// 	{165,97},{167,98},{169,100},{172,101},{174,102},{176,104},{179,105},{181,106},{183,108},{185,109},
// 	{188,110},{190,112},{192,113},{195,115},{197,116},{199,117},{202,119},{204,120},{206,121},{209,123},
// 	{211,124},{213,125},{216,127},{218,128},{220,130},{223,131},{225,132},{227,134},{230,135},{232,136},
// 	{234,138},{236,139},{239,140},{241,142},{243,143},{246,145},{248,146},{250,147},{253,149},{255,150},
// 		
// 	{2,1},{5,3},{7,4},{9,5},{12,7},{14,8},{16,10},{19,11},{21,12},{23,14},
// 	{26,15},{28,16},{30,18},{32,19},{35,20},{37,22},{39,23},{42,25},{44,26},{46,27},
// 	{49,29},{51,30},{53,31},{56,33},{58,34},{60,35},{63,37},{65,38},{67,40},{70,41},
// 	{72,42},{74,44},{77,45},{79,46},{81,48},{83,49},{86,50},{88,52},{90,53},{93,55},
// 	{95,56},{97,57},{100,59},{102,60},{104,61},{107,63},{109,64},{111,65},{114,67},{116,68},
// 	{118,70},{121,71},{123,72},{125,74},{128,75},{130,76},{132,78},{134,79},{137,80},{139,82},
// 	{141,83},{144,85},{146,86},{148,87},{151,89},{153,90},{155,91},{158,93},{160,94},{162,95},
// 	{165,97},{167,98},{169,100},{172,101},{174,102},{176,104},{179,105},{181,106},{183,108},{185,109},
// 	{188,110},{190,112},{192,113},{195,115},{197,116},{199,117},{202,119},{204,120},{206,121},{209,123},
// 	{211,124},{213,125},{216,127},{218,128},{220,130},{223,131},{225,132},{227,134},{230,135},{232,136},
// 	{234,138},{236,139},{239,140},{241,142},{243,143},{246,145},{248,146},{250,147},{253,149},{255,150}		
// };

char led_tbl[41][2] = {
	{240,150},	{221,139},	{204,127},	{187,117},	{172,108},	{159,99},	{146,91},	{134,84},	{124,77},	{114,71},
	{105,66},	{96,60},	{89,56},	{82,51},	{75,47},	{69,43},	{64,40},	{59,37},	{54,34},	{50,31},
	{46,29},	{42,27},	{39,24},	{36,23},	{33,21},	{30,19},	{28,18},	{26,16},	{24,15},	{22,14},
	{20,13},	{19,12},	{17,11},	{16,10},	{15,9},	{13,9},	{12,8},	{11,7},	{11,7},	{10,6},
	{9,6}
};

char led_tbl_sec[82][2] = {
	{240,150},	{221,139},	{204,127},	{187,117},	{172,108},	{159,99},	{146,91},	{134,84},	{124,77},	{114,71},
	{105,66},	{96,60},	{89,56},	{82,51},	{75,47},	{69,43},	{64,40},	{59,37},	{54,34},	{50,31},
	{46,29},	{42,27},	{39,24},	{36,23},	{33,21},	{30,19},	{28,18},	{26,16},	{24,15},	{22,14},
	{20,13},	{19,12},	{17,11},	{16,10},	{15,9},	{13,9},	{12,8},	{11,7},	{11,7},	{10,6},	
	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},
	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},
	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},	{9,6},
	{9,6},{9,6},	{9,6},	{9,6},	{9,6},

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
//				sprintf(temp, "--- main_mode  :  %dmin OFF  ---\n", time_min);
//				USART_Transmit_str(temp, _ASCII);
				main_btn_mode  = MODE_DEFAULT;
				led_btn_mode   = MODE_DEFAULT;
				timer_btn_mode = MODE_DEFAULT;
			}
		}
		else if(timer_btn_mode == 2)
		{
			if(time_min == TIME_SET_B)
			{
//				sprintf(temp, "--- main_mode  :  %dmin OFF  ---\n", time_min);
//				USART_Transmit_str(temp, _ASCII);
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
			if(main_btn_mode == 0)
			{
				main_btn_mode = 1;			// 전원 ON
				led_btn_mode = 1;			// led mode = ON
				dimming = 1;
//				sprintf(temp, "--- main_mode  :  ON  ---\n");
			}
			else 
			{
				main_btn_mode  = MODE_DEFAULT;
				led_btn_mode   = MODE_DEFAULT;
				timer_btn_mode = MODE_DEFAULT;
//				sprintf(temp, "--- main_mode  :  OFF ---\n");
			}
//			USART_Transmit_str(temp, _ASCII);
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
			if(led_btn_cnt > 18)
			{
				if(led_btn_mode == 1 || led_btn_mode == 2)
				{
					if(dimming_mode == 0)
					{
//						if(dimming > 0)
//						{
							dimming = dimming + 1;
							if(dimming > 59)	dimming = 60;
//							if(dimming > 149)	dimming = 150;
//						}
					}
					else
					{
//						if(dimming < 200)
//						{
							dimming = dimming - 1;
							if(dimming < 2)		dimming = 1;
//						}
					}
				}
			}
		}
	}
	else
	{
		led_btn_flag = 0;
		if(led_btn_cnt)
		{
			if(led_btn_cnt > 18)
			{
 				if(led_btn_mode == 1 || led_btn_mode == 2)
 				{
 					if(dimming_mode == 0)	dimming_mode = 1;
					else                    dimming_mode = 0;
 				}
//				if(led_btn_mode == 1)		sprintf(temp, "--- led_mode  :  ON  ---,  dimming = %d\n", dimming);
//				else if(led_btn_mode == 2)	sprintf(temp, "--- led_mode  :  ANI ---,  dimming = %d\n", dimming);
//				USART_Transmit_str(temp, _ASCII);
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
// 					if(led_btn_mode == 0)		sprintf(temp, "--- led_mode  :  OFF ---,  dimming = %d\n", dimming);
// 					else if(led_btn_mode == 1)	sprintf(temp, "--- led_mode  :  ON  ---,  dimming = %d\n", dimming);
// 					else						sprintf(temp, "--- led_mode  :  ANI ---,  dimming = %d\n", dimming);
// 					USART_Transmit_str(temp, _ASCII);
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
				if(timer_btn_mode == 0)
				{
					PORTC &= ~(1 << PORTC2);
					PORTC &= ~(1 << PORTC3);
//					sprintf(temp, "--- timer_mode  = Timer off     ---\n");
				}
				else if(timer_btn_mode == 1)
				{
					PORTC |= (1 << PORTC2);
					PORTC &= ~(1 << PORTC3);
//					sprintf(temp, "--- timer_mode  = Timer 30min   ---\n");
				}
				else
				{
					PORTC &= ~(1 << PORTC2);
					PORTC |= (1 << PORTC3);
//					sprintf(temp, "--- timer_mode  = Timer 60min   ---\n");
				}
//				USART_Transmit_str(temp, _ASCII);
			}
		}
		timer_btn_cnt = 0;
	}
}


void led_control(char num)
{
	int i, j;
	if(num == 2)
	{
// 		for(j=0;j<LED_NUM;j++)
// 		{
// 			for(i=0;i<LED_NUM;i++)
// 			{
//  				led[i].r = led_tbl[i+j][0];
//  				led[i].g = led_tbl[i+j][1];
//  				led[i].b = 0;
// // 				unsigned char a = rand()%100;
// // 				led[i].r = 255 / a;
// // 				led[i].g = 150 / a;
// // 				led[i].b = 0;
// 			}
// 			ws2812_setleds(led, LED_NUM);
// 			_delay_ms(30);
// 			if(led_btn_mode != 2)	break;
// 		}
		if(ani_state == 0)
		{
			ani_state = 1;
			for(j=0; j<LED_NUM/2; j++)
			{
				for(i=0; i<9; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=9; i<18; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=18; i<27; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=27; i<36; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=36; i<44; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=44; i<53; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=53; i<62; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=62; i<71; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=71; i<80; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				ws2812_setleds(led, LED_NUM);
				_delay_ms(70);
				if(led_btn_mode != 2)	break;
			}
		}
		else
		{
			ani_state = 0;
			for(j=0; j<LED_NUM/2; j++)
			{
				for(i=0; i<9; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=9; i<18; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=18; i<27; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=27; i<36; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=36; i<44; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=44; i<53; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=53; i<62; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				for(i=62; i<71; i++)
				{
					led[i].r = led_tbl[j][0];
					led[i].g = led_tbl[j][1];
					led[i].b = 0;
				}
				for(i=71; i<80; i++)
				{
					led[i].r = led_tbl[(LED_NUM/2)-j][0];
					led[i].g = led_tbl[(LED_NUM/2)-j][1];
					led[i].b = 0;
				}
				ws2812_setleds(led, LED_NUM);
				_delay_ms(70);
				if(led_btn_mode != 2)	break;
			}
		}
	}
}


void led_display(void)
{
	for(int i=0;i<LED_NUM;i++)
	{
		led[i].r = led_tbl_sec[dimming][0];
		led[i].g = led_tbl_sec[dimming][1];
//		led[i].r = (uint8_t)(240 / dimming);
//		led[i].g = (uint8_t)(150 / dimming);
		led[i].b = 0;		
	}
	ws2812_setleds(led, LED_NUM);
}

void led_off(void)
{
	for(int i=0;i<LED_NUM;i++)
	{
		led[i].r = 0;
		led[i].g = 0;
		led[i].b = 0;
	}
	ws2812_setleds(led, LED_NUM);
}


int main(void)
{
    DDRB = (1 << PORTB0) | (1 << PORTB5);		// PORTB0 = WS2812B, PORTB5 = status debug
	DDRC = (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4);
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
//	sprintf(temp, "--- Now Start ---\n");
//	USART_Transmit_str(temp, _ASCII);
	sei();
    while (1)
    {
  		if(main_btn_mode == 1 && led_btn_mode == 1)
  		{
 			led_display();
		}
		else if(main_btn_mode == 1 && led_btn_mode == 2)
		{
	 		led_control(2);
		}
		else if(main_btn_mode == 0 || led_btn_mode == 0)
		{
			led_off();
			PORTB &= ~(1 << PORTB5);
			PORTC &= ~(1 << PORTC2);
			PORTC &= ~(1 << PORTC3);
		}
    }
}
