/*
 * HardwarePWM.c
 * For Atmega64m1 Only
 * Created: 2019/4/8 上午 01:44:05
 *  Author: 王碩傑
 */ 
#include "../inc/HardwarePWM_Lib.h"

/*
while accessing timer1 16bit register needed atomic operation
to avoid interrupt while accessing the register
OCR1A_En can have two PWM output, however it will not have the double buffered protection 
*/

void HardwarePWM_init(unsigned int PWM_Period,unsigned int OCR1A_EN) //OCR1A_EN 1, for double output
{
	DDRD |= _BV(2); //set OCR1A(PD2) as output
	DDRC |= _BV(1); //set OCR1B(PC1) as output
	
	TIMSK1 &= ~_BV(OCIE1A);//disable OC1A interrupt
	TIMSK1 &= ~_BV(OCIE1B);//disable OC1B interrupt
	
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	//prescale /1024:101 /256:100 /8:010
	TCCR1B &= ~_BV(CS12);
	TCCR1B |= _BV(CS11);
	TCCR1B &= ~_BV(CS10);
	
	if(OCR1A_EN)
	{
		//Waveform generate Phase and frequency correct ICR1: 1000
		TCCR1B |= _BV(WGM13);
		TCCR1B &= ~_BV(WGM12);
		TCCR1A &= ~_BV(WGM11);
		TCCR1A &= ~_BV(WGM10);
		
		ICR1 = PWM_Period;
		OCR1A = 0;
		OCR1B = 0;
	}
	else
	{
		//Waveform generate Phase and frequency correct OCR1A: 1001
		TCCR1B |= _BV(WGM13);
		TCCR1B &= ~_BV(WGM12);
		TCCR1A &= ~_BV(WGM11);
		TCCR1A |= _BV(WGM10);
		
		OCR1A = PWM_Period;
		OCR1B = 0;
	}
	
	//Connect OCCR1 A/B to _PD2/_PC1 falling edge on rising edge off : 10
	TCCR1A |= _BV(COM1A1);
	TCCR1A &= ~_BV(COM1A0);
	TCCR1A |= _BV(COM1B1);
	TCCR1A &= ~_BV(COM1B0);
	
	TIMSK1 |= _BV(OCIE1A);//enable OC1A interrupt
	TIMSK1 |= _BV(OCIE1B);//enable OC1B interrupt
}