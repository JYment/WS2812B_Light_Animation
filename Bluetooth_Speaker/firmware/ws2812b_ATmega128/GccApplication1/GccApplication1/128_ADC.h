/*
 * _128_ADC.h
 *
 * Created: 19-11-11 월 오후 02:48:07
 *  Author: LABC
 */ 


#define EXTERNAL	0			// AREF PIN
#define DEFAULT		1			// 5V
#define INTERNAL	3			// 1.1V

unsigned char analog_ref = DEFAULT;

void j_analogReference(char mode)
{
	if(mode == EXTERNAL)			analog_ref = EXTERNAL;
	else if(mode == INTERNAL)		analog_ref = INTERNAL;
	else							analog_ref = DEFAULT;
}

unsigned short j_analogRead(char adc_pin)
{
	unsigned char low, high;
	
	ADMUX = (analog_ref << 6) | (adc_pin & 0x1F);
	ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);		// 128분주 ADPS('111'), 0.125Mhz
	ADCSRA |= (1 << ADSC);

	while ((ADCSRA & 0x10) != 0x10);
	low = ADCL;
	high = ADCH;
	
	return (high << 8) | low;
}
