/*
 * Atmega64m1_Core.c
 *
 * Created: 2019/4/4 上午 02:06:36
 * Author : 王碩傑
 */ 
#define F_CPU 16000000UL
#include "CTInstruments.h"

volatile unsigned long RTC_ms, RTC_s, RTC_100us;// Buffered RTC
volatile unsigned long _RTC_ms, _RTC_s, _RTC_100us;
volatile unsigned long _RTC_Tim0OVF_Count;//Private only for micros()
volatile uint8_t _RTC_Tim0OVF_Count1;//Private only for ms++
volatile uint8_t _RTC_Tim0OVF_Count2;//Private only for s++
volatile uint32_t _Control_Flag_0,_Control_Flag_1,_Control_Flag_2,_Control_Flag_3,_Control_Flag_4;
volatile byte RTC_Flag = 0x00;

int main(void)
{
    init();
	setup();
	while (1)
	{
		wdt_reset();
		loop();
		//if(_Control_Flag_0 > 60000) = 0;
	}
}

void init(void)
{
	WatchDog_init();
	sei();
	GPIO_select();
	//Initialize RTC Variables
	RTC_100us = 0;
	RTC_ms = 0;
	RTC_s = 0;
	_RTC_Tim0OVF_Count = 0;
	_RTC_Tim0OVF_Count1 = 0;
	_RTC_Tim0OVF_Count2 = 0;
	_RTC_100us = 0;
	_RTC_ms = 0;
	_RTC_s = 0;
	//Initialize Control Flag
	_Control_Flag_0 = 0;
	_Control_Flag_1 = 0;
	_Control_Flag_2 = 0;
	_Control_Flag_3 = 0;
	_Control_Flag_4 = 0;
	
	RTC_Flag = 0x00;//Clear RTC R/W buffer mask
	
	cbi(TIMSK0, TOIE0); //diable Timer 0 Overflow interrupt
	//Initialize timer0
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	//set Timer0 as simple counter
	
	/*
	//set Timer0 as fast PWM Mode as Arduino
	#if defined(TCCR0A) && defined(WGM01)
		sbi(TCCR0A,WGM01);
		sbi(TCCR0A,WGM00);
	#endif
	*/
	//set timer 0 prescale factor to 64
	#if defined(TCCR0B) && defined(CS01) && defined(CS00)
		sbi(TCCR0B,CS01);
		sbi(TCCR0B,CS00);
	#else
		#error Timer 0 prescale factor 64 not set correctly
	#endif
	TCNT0 = TCNT0_RST; //shorten to ovf every 100us
	
	// enable timer 0 overflow interrupt
	#if defined(TIMSK) && defined(TOIE0)
		sbi(TIMSK, TOIE0);
	#elif defined(TIMSK0) && defined(TOIE0)
		sbi(TIMSK0, TOIE0);
	#else
		#error	Timer 0 overflow interrupt not set correctly
	#endif
	
	cbi(TIMSK1,TOIE1);
	//Initialize timer1
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	//set Timer1 as simple counter and disable ocr1A/B
	
	//set timer 1 prescale factor to 8
	#if defined(TCCR1B) && defined(CS11)
		sbi(TCCR1B,CS11);
	#else
		#error Timer 0 prescale factor 64 not set correctly
	#endif
	
	//ADC initialization
	#if defined(ADCSRA)
	// set a2d prescaler so we are inside the desired 50-200 KHz range for better resolution.
		#if F_CPU >= 16000000 // 16 MHz / 128 = 125 KHz
			sbi(ADCSRA, ADPS2);
			sbi(ADCSRA, ADPS1);
			sbi(ADCSRA, ADPS0);
		#elif F_CPU >= 8000000 // 8 MHz / 64 = 125 KHz
			sbi(ADCSRA, ADPS2);
			sbi(ADCSRA, ADPS1);
			cbi(ADCSRA, ADPS0);
		#elif F_CPU >= 4000000 // 4 MHz / 32 = 125 KHz
			sbi(ADCSRA, ADPS2);
			cbi(ADCSRA, ADPS1);
			sbi(ADCSRA, ADPS0);
		#elif F_CPU >= 2000000 // 2 MHz / 16 = 125 KHz
			sbi(ADCSRA, ADPS2);
			cbi(ADCSRA, ADPS1);
			cbi(ADCSRA, ADPS0);
		#elif F_CPU >= 1000000 // 1 MHz / 8 = 125 KHz
			cbi(ADCSRA, ADPS2);
			sbi(ADCSRA, ADPS1);
			sbi(ADCSRA, ADPS0);
		#else // 128 kHz / 2 = 64 KHz -> This is the closest you can get, the prescaler is 2
			cbi(ADCSRA, ADPS2);
			cbi(ADCSRA, ADPS1);
			sbi(ADCSRA, ADPS0);
		#endif
		// enable a2d conversions
		sbi(ADCSRA, ADEN);
	#endif
	
	#if defined(UCSRB)
		UCSRB = 0;
	#elif defined(UCSR0B)
		UCSR0B = 0;
	#elif defined(LINCR)
	// Reset the LIN UART (AtmegaxxM1/C1) back to default parameters
		sbi(LINCR, LSWRES);
	#endif
	
	//SPI_init(); //T.B.C.
	//CAN_init(); //T.B.C.
}

void WatchDog_init(void)
{
	cli(); //pause all interrupt
	wdt_reset();//Reset WD Timer
	MCUSR &= ~(1 << WDRF); //Clear WD reset flag
	WDTCSR |= (1<<WDCE) | (1<<WDE); //Start timed sequence
	WDTCSR = (1<<WDE) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0); //in four clock cycle set WD psc to 2s
	WDTCSR |= (1<<WDIF); //clear WD interrupt flag
	WDTCSR |= (1<<WDIE); //Enable WD interrupt for safety shutdown
	
	sei(); //start interrupt
}

void SPI_init()
{
	//FRam_init(); //T.B.C Need to confrim the Mission Time/State Machine allignment Sequence
}

void CAN_init() //int
{
	
}

//volatile bool T_State=true;
ISR(TIMER0_OVF_vect)
{
	//GPIO_DW_HIGH(_PD4); //for timing
	TCNT0 = TCNT0_RST;
	_RTC_Tim0OVF_Count++; //Count the overall Tim0 OVF times -> used in micros()
	_RTC_Tim0OVF_Count1++; //Count the overall Tim0 OVF times -> used in ms++
	_RTC_Tim0OVF_Count2++; //Count the overall Tim0 OVF times -> used in s++
	_RTC_100us += RTC_PERIOD_100US;
	
	if(_RTC_Tim0OVF_Count1 > 9) //per 10 100us
	{
		_RTC_ms++;
		_RTC_Tim0OVF_Count1 = 0;
	}
	if(_RTC_Tim0OVF_Count2 > 9999) //per 10000 100us
	{
		_RTC_s++;
		_RTC_Tim0OVF_Count2 = 0;
	}
	
	if(!_Control_Flag_0) _Control_Flag_0 = 0;
	if(!_Control_Flag_1) _Control_Flag_1 = 0;
	if(!_Control_Flag_2) _Control_Flag_2 = 0;
	if(!_Control_Flag_3) _Control_Flag_3 = 0;
	if(!_Control_Flag_4) _Control_Flag_4 = 0;
	
	SRTC(RTC0); RTC_100us = _RTC_100us;
	SRTC(RTC1); RTC_ms = _RTC_ms;
	SRTC(RTC2); RTC_s = _RTC_s;
	SRTC(CF0); _Control_Flag_0++;
	SRTC(CF1); _Control_Flag_1++;
	SRTC(CF2); _Control_Flag_2++;
	SRTC(CF3); _Control_Flag_3++;
	SRTC(CF4); _Control_Flag_4++;
	
	/*
	//T.B.D if using CF as uint16_t
	//Overflow Protection
	if(_Control_Flag_0 > Control_Flag_Max) _Control_Flag_0 = 0;
	if(_Control_Flag_1 > Control_Flag_Max) _Control_Flag_0 = 1;
	if(_Control_Flag_2 > Control_Flag_Max) _Control_Flag_0 = 2;
	if(_Control_Flag_3 > Control_Flag_Max) _Control_Flag_0 = 3;
	if(_Control_Flag_4 > Control_Flag_Max) _Control_Flag_0 = 4;
	*/
	
	/*
	//Put into buffer if no one else reading it
	if(Check_RTC(RTC0)) RTC_100us = _RTC_100us;
	if(Check_RTC(RTC1)) RTC_ms = _RTC_ms;
	if(Check_RTC(RTC2)) RTC_s = _RTC_s;
	if(Check_RTC(CF0)) Control_Flag_0 = _Control_Flag_0;
	if(Check_RTC(CF1)) Control_Flag_1 = _Control_Flag_1;
	if(Check_RTC(CF2)) Control_Flag_2 = _Control_Flag_2;
	if(Check_RTC(CF3)) Control_Flag_3 = _Control_Flag_3;
	if(Check_RTC(CF4)) Control_Flag_4 = _Control_Flag_4;
	*/
	
	//GPIO_DW_LOW(_PD4); //for timing
	/*
	if(!T_State) T_State=true;
	else T_State = false;
	if(T_State) GPIO_DW_HIGH(_PB6);//GPIO_DO_HIGH(LEDPort,LEDMsk);
	else GPIO_DW_LOW(_PB6);//GPIO_DO_LOW(LEDPort,LEDMsk);
	*/
}

ISR(WDT_vect)
{
	Error_Handler();//Error Handler
}

unsigned long millis()
{
	unsigned long m;
	do
	{
		FRTC(RTC1);
		wdt_reset();
		m = RTC_ms;
	}while(Check_RTC(RTC1)); //if somebody changed RTC
	SRTC(RTC1);
	return m;
}

unsigned long micros_100()
{
	unsigned long us;
	do 
	{
		FRTC(RTC0);
		wdt_reset();
		us = RTC_100us;
	} while(Check_RTC(RTC0));
	SRTC(RTC0);
	return us;
}

unsigned long seconds()
{
	unsigned long s;
	do
	{
		FRTC(RTC2);
		wdt_reset();
		s = RTC_s;
	} while(Check_RTC(RTC2));
	SRTC(RTC2);
	return s;
}

unsigned long Control_Flag_0()
{
	unsigned long CF;
	do
	{
		FRTC(CF0);
		wdt_reset();
		CF = _Control_Flag_0;
	} while(Check_RTC(CF0));
	SRTC(CF0);
	return CF;
}

unsigned long Control_Flag_1()
{
	unsigned long CF;
	do
	{
		FRTC(CF1);
		wdt_reset();
		CF = _Control_Flag_1;
	} while(Check_RTC(CF1));
	SRTC(CF1);
	return CF;
}

unsigned long Control_Flag_2()
{
	unsigned long CF;
	do
	{
		FRTC(CF2);
		wdt_reset();
		CF = _Control_Flag_2;
	} while(Check_RTC(CF2));
	SRTC(CF2);
	return CF;
}

unsigned long Control_Flag_3()
{
	unsigned long CF;
	do
	{
		FRTC(CF3);
		wdt_reset();
		CF = _Control_Flag_3;
	} while(Check_RTC(CF3));
	SRTC(CF3);
	return CF;
}

unsigned long Control_Flag_4()
{
	unsigned long CF;
	do
	{
		FRTC(CF4);
		wdt_reset();
		CF = _Control_Flag_4;
	} while(Check_RTC(CF4));
	SRTC(CF4);
	return CF;
}

void delay(unsigned long duration)
{
	//RTC aquiring sequence
	unsigned long Delay_LTC;
	do
	{
		FRTC(RTC0);
		wdt_reset();
		Delay_LTC = RTC_100us;
	} while(Check_RTC(RTC0));
	SRTC(RTC0);
	//End of RTC aquiring
	duration *= 10; //convert to 100us
	unsigned long Delay_ETC = Delay_LTC + duration;//Delay End time clock
	unsigned long RTC = micros_100();
	while(RTC <= Delay_ETC)
	{
		if((RTC-Delay_LTC)>=100) //reset WD every 10 ms
		{
			wdt_reset();
			Delay_LTC += 100;
		}
		RTC = micros_100();
	}
}