/*
 * Software_PWM.c
 *
 * Created: 2019/4/8 上午 01:44:05
 *  Author: 王碩傑
 */ 
#include "../inc/SoftwarePWM_Lib.h"

/*
while accessing timer1 16bit register needed atomic operation
to avoid interrupt while accessing the register

*/

volatile int PWMPeriod = 64; //PWMCycle 3.2ms (改成0.05ms一次interrupt)
#define CLK1RST 64136 //0.05ms一次interrupt
volatile int PWMCounter;
volatile int PWMValue[8] = {0,0,0,0,0,0,0,0};
volatile byte PWM_Mask = 0x00;
volatile unsigned char TCNT1_sreg;

void SPWM_init(void) //OCR1A_EN 1, for double output
{	
	//Pin Configuration
	if(PWM_Mask & _BV(PWM0)) GPIO_MODE_OUTPUT(PWM_Pin0);
	if(PWM_Mask & _BV(PWM1)) GPIO_MODE_OUTPUT(PWM_Pin1);
	if(PWM_Mask & _BV(PWM2)) GPIO_MODE_OUTPUT(PWM_Pin2);
	if(PWM_Mask & _BV(PWM3)) GPIO_MODE_OUTPUT(PWM_Pin3);
	if(PWM_Mask & _BV(PWM4)) GPIO_MODE_OUTPUT(PWM_Pin4);
	if(PWM_Mask & _BV(PWM5)) GPIO_MODE_OUTPUT(PWM_Pin5);
	if(PWM_Mask & _BV(PWM6)) GPIO_MODE_OUTPUT(PWM_Pin6);
	if(PWM_Mask & _BV(PWM7)) GPIO_MODE_OUTPUT(PWM_Pin7);
	
	//Timer 1 Init
	TIMSK1 &= ~_BV(TOIE1);//disable OC1B interrupt
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	
	//prescale /1024:101 /256:100 /8:010
	TCCR1B &= ~_BV(CS12);
	TCCR1B |= _BV(CS11);
	TCCR1B &= ~_BV(CS10);

	//Atomic write to 16bit TCNT1
	TCNT1_sreg = SREG;
	cli();
	TCNT1 = CLK1RST;
	SREG = TCNT1_sreg;
	sei();
	
	
	TIMSK1 |= _BV(TOIE1);//enable timer 1 
	
}

//#ifdef SoftwarePWM_LIB_H_

/*
volatile bool T_State= true;

ISR(TIMER1_OVF_vect)
{
	//Atomic write to 16bit TCNT1
	TCNT1_sreg = SREG;
	cli();
	TCNT1 = -47;
	SREG = TCNT1_sreg;
	sei();
	if(!T_State) T_State=true;
	else T_State = false;
	if(T_State) GPIO_DW_HIGH(PWM_Pin0);//GPIO_DO_HIGH(LEDPort,LEDMsk);
	else GPIO_DW_LOW(PWM_Pin0);//GPIO_DO_LOW(LEDPort,LEDMsk);
}
*/

volatile boolean LED_state = 1;

ISR(TIMER1_OVF_vect)
{
	TCNT1L = CLK1RST&0xFF;
	TCNT1H = (CLK1RST>>8)&0xFF;
	if(LED_state)
	{
		GPIO_HIGH(_PD4);
		LED_state = 0;
	}
	else
	{
		GPIO_LOW(_PD4);
		LED_state = 1;
	}
	//Atomic write to 16bit TCNT1
	//TCNT1_sreg = SREG;
	//SREG = TCNT1_sreg;
	PWMCounter++;
	if(PWMCounter >= PWMPeriod)
	{
		if(PWM_Mask & _BV(PWM0)) GPIO_HIGH(PWM_Pin0);
		if(PWM_Mask & _BV(PWM1)) GPIO_HIGH(PWM_Pin1);
		if(PWM_Mask & _BV(PWM2)) GPIO_HIGH(PWM_Pin2);
		if(PWM_Mask & _BV(PWM3)) GPIO_HIGH(PWM_Pin3);
		if(PWM_Mask & _BV(PWM4)) GPIO_HIGH(PWM_Pin4);
		if(PWM_Mask & _BV(PWM5)) GPIO_HIGH(PWM_Pin5);
		if(PWM_Mask & _BV(PWM6)) GPIO_HIGH(PWM_Pin6);
		if(PWM_Mask & _BV(PWM7)) GPIO_HIGH(PWM_Pin7);
		PWMCounter = 0;
	}
	if((PWM_Mask & _BV(PWM0)) && PWMCounter >= PWMValue[0]) GPIO_LOW(PWM_Pin0);//PWMLow
	if((PWM_Mask & _BV(PWM1)) && PWMCounter >= PWMValue[1]) GPIO_LOW(PWM_Pin1);//PWMLow
	if((PWM_Mask & _BV(PWM2)) && PWMCounter >= PWMValue[2]) GPIO_LOW(PWM_Pin2);//PWMLow
	if((PWM_Mask & _BV(PWM3)) && PWMCounter >= PWMValue[3]) GPIO_LOW(PWM_Pin3);//PWMLow
	if((PWM_Mask & _BV(PWM4)) && PWMCounter >= PWMValue[4]) GPIO_LOW(PWM_Pin4);//PWMLow
	if((PWM_Mask & _BV(PWM5)) && PWMCounter >= PWMValue[5]) GPIO_LOW(PWM_Pin5);//PWMLow
	if((PWM_Mask & _BV(PWM6)) && PWMCounter >= PWMValue[6]) GPIO_LOW(PWM_Pin6);//PWMLow
	if((PWM_Mask & _BV(PWM7)) && PWMCounter >= PWMValue[7]) GPIO_LOW(PWM_Pin7);//PWMLow
	
}

//#endif