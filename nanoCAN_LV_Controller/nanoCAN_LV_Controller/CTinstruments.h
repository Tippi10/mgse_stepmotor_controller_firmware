/*
 * CTinstruments.h
 *
 * Created: 2019/4/4 上午 02:52:38
 *  Author: 王碩傑
 */ 


#ifndef CTINSTRUMENTS_H_
#define CTINSTRUMENTS_H_

#ifdef __cplusplus
extern "C" {
#endif	
	
//======== Include Needed Headers ============
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>	

#include "../Arduino_Lib/binary.h"

#include "GPIO_Map.h"

//====== Default Real-Time clock const =====
#define USE_EXT_CLK 0
//#define F_CPU 8000000UL // T.B.D. already define at the beginning of the CTinstruments.c
#define RTC_PERIOD_100US 1
#define RTC_PERIOD_MS 1
#define TCNT0_RST -25
//T.B.D. will improve speed but decrease compatibility
//#define Control_Flag_Max 60000

//Flag RTC0:100us RTC1:ms RTC2:s
#define RTC0 0
#define RTC1 1
#define RTC2 2
#define CF0 3
#define CF1 4
#define CF2 5
#define CF3 6
#define CF4 7

//====== Default Function ==================
#define _GPIO_MODE_OUTPUT(DDRX,PORT,PIN,MSK,PWM) (DDRX |= _BV(MSK))
#define _GPIO_MODE_INPUT(DDRX,PORT,PIN,MSK,PWM) (DDRX &= ~_BV(MSK))
#define _GPIO_HIGH(DDRX,PORT,PIN,MSK,PWM)  (PORT |= _BV(MSK))
#define _GPIO_LOW(DDRX,PORT,PIN,MSK,PWM) (PORT &= ~_BV(MSK))
#define _GPIO_DREAD(DDRX,PORT,PIN,MSK,PWM) (PIN & MSK)

#define GPIO_MODE_OUTPUT(PXX) _GPIO_MODE_OUTPUT(PXX)
#define GPIO_MODE_INPUT(PXX) _GPIO_MODE_INPUT(PXX)
#define GPIO_HIGH(PXX)  _GPIO_HIGH(PXX)
#define GPIO_LOW(PXX) _GPIO_LOW(PXX)
#define GPIO_DREAD(PXX) _GPIO_DREAD(PXX)

#define wdt_reset() __asm__ __volatile__ ("wdr")
//volatile need __volatile__? as wdt.h says?  asm wdr can use or not

#define SRTC(BIT) (RTC_Flag |= _BV(BIT))
//Set RTC flag
#define FRTC(BIT) (RTC_Flag &= ~_BV(BIT))
//Free RTC 
#define Check_RTC(BIT) !(RTC_Flag & _BV(BIT))
//Check RTC for ISR only

//Clear Control flag;
#define CCF0() _Control_Flag_0 = 0
#define CCF1() _Control_Flag_1 = 0
#define CCF2() _Control_Flag_2 = 0
#define CCF3() _Control_Flag_3 = 0
#define CCF4() _Control_Flag_4 = 0

//======== Define Common Math Constant =======
#define pi 3.141592653589793
#define math_e 2.718281828459045

//======== Define Common Arduino Constant ====
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define CHANGE 1
#define FALLING 2
#define RISING 3

//======= Define Common Arduino Function ====
#define interrupts() sei()
#define noInterrupts() cli()

#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

#define bit(b) (1UL << (b))

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//======= Define Common Arduino Typedef =====
typedef unsigned int word;
typedef bool boolean;
typedef uint8_t byte;

//======= The Using Functions ===============
void GPIO_select(void);             // necessary in main.c
void init(void);                    // can only appear in CTInstruments.c, not a public function
void setup(void);                   // necessary in main.c
void loop(void);
void WatchDog_init(void);           //WatchDog Timer init
void Error_Handler(void);           //Error Handler before WatchDog reset occurs

//======= Common Using Functions ============
unsigned long millis(void);
unsigned long micros_100(void);
unsigned long seconds(void);
unsigned long Control_Flag_0(void);
unsigned long Control_Flag_1(void);
unsigned long Control_Flag_2(void);
unsigned long Control_Flag_3(void);
unsigned long Control_Flag_4(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);

//======= Task Sensitive Functoin ===========

void SPI_init(void);
void FRam_init(void);
void CAN_init(void);

	
#ifdef __cplusplus	
}
#endif // extern "C"

#endif /* CTINSTRUMENTS_H_ */