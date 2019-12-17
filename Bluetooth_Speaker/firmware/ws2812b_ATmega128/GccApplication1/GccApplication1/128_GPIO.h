/*
 * _128_GPIO.h
 *
 * Created: 19-11-11 월 오후 01:23:57
 *  Author: LABC
 */ 


#define INPUT		1
#define OUTPUT		0
#define HIGH		1
#define LOW			0

void j_pinMode(char port, char port_num, char mode)
{
	if(mode == OUTPUT)
	{
		if(port == 'A')			DDRA |= (1 << port_num);
		else if(port == 'B')	DDRB |= (1 << port_num);
		else if(port == 'C')	DDRC |= (1 << port_num);
		else if(port == 'D')	DDRD |= (1 << port_num);
		else if(port == 'E')	DDRE |= (1 << port_num);
		else if(port == 'F')	DDRF |= (1 << port_num);
		else if(port == 'G')	DDRG |= (1 << port_num);
	}
	else if(mode == INPUT)
	{
		if(port == 'A')			DDRA &= ~(1 << port_num);
		else if(port == 'B')	DDRB &= ~(1 << port_num);
		else if(port == 'C')	DDRC &= ~(1 << port_num);
		else if(port == 'D')	DDRD &= ~(1 << port_num);
		else if(port == 'E')	DDRE &= ~(1 << port_num);
		else if(port == 'F')	DDRF &= ~(1 << port_num);
		else if(port == 'G')	DDRG &= ~(1 << port_num);
	}
}

void j_digitalWrite(char port, char port_num, char sig)
{
	if(sig == HIGH)
	{
		if(port == 'A')			PORTA |= (1 << port_num);
		else if(port == 'B')	PORTB |= (1 << port_num);
		else if(port == 'C')	PORTC |= (1 << port_num);
		else if(port == 'D')	PORTD |= (1 << port_num);
		else if(port == 'E')	PORTE |= (1 << port_num);
		else if(port == 'F')	PORTF |= (1 << port_num);
		else if(port == 'G')	PORTG |= (1 << port_num);
	}
	else if(sig == LOW)
	{
		if(port == 'A')			PORTA &= ~(1 << port_num);
		else if(port == 'B')	PORTB &= ~(1 << port_num);
		else if(port == 'C')	PORTC &= ~(1 << port_num);
		else if(port == 'D')	PORTD &= ~(1 << port_num);
		else if(port == 'E')	PORTE &= ~(1 << port_num);
		else if(port == 'F')	PORTF &= ~(1 << port_num);
		else if(port == 'G')	PORTG &= ~(1 << port_num);
	}
}


/* 	Switch - PULL UP   설계   - Pressed : return 0
	Switch - PULL DOWN 설계   - Pressed : return 1	*/
char j_digitalRead(char port, char port_num)
{
	if(port == 'A')			return PINA & (1 << port_num);
	else if(port == 'B')	return PINB & (1 << port_num);
	else if(port == 'C')	return PINC & (1 << port_num);
	else if(port == 'D')	return PIND & (1 << port_num);
	else if(port == 'E')	return PINE & (1 << port_num);
	else if(port == 'F')	return PINF & (1 << port_num);
	else if(port == 'G')	return PING & (1 << port_num);
	else return 0;
}
