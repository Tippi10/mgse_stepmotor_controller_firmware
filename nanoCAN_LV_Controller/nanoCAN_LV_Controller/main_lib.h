/*
 * main_lib.h
 *
 * Created: 2019/4/4 上午 09:07:59
 *  Author: 王碩傑
 */ 


#ifndef MAIN_LIB_H_
#define MAIN_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

//define using header

#include "CTinstruments/CTInstruments.h"
#include "inc/CAN.h"

//define the using GPIO port
#define LED _PD2
#define GPIO_Test _PB6

#define stepPin1 _PD4
#define enPin1 _PD5
#define dirPin1 _PD6
#define stepPin2 _PD1
#define enPin2 _PD2
#define dirPin2 _PC1
#define stepPin3 _PB3
#define enPin3 _PB4
#define dirPin3 _PC7
#define stepPin4 _PB5
#define enPin4 _PB6
#define dirPin4 _PD0

#define V12_ctrl _PC0

#define multA _PB2
#define multB _PC4
#define phaseA _PC5
#define phaseB _PD7

//define using extern variables
extern volatile unsigned long RTC_ms, RTC_s, RTC_100us;
extern volatile uint32_t _Control_Flag_0,_Control_Flag_1,_Control_Flag_2,_Control_Flag_3,_Control_Flag_4;
extern volatile byte RTC_Flag;
extern volatile int PWMPeriod;
extern volatile int PWMValue[8];
extern volatile byte PWM_Mask;
extern uint8_t CAN_MSG[8];
extern uint8_t CAN_DATATOSEND[8];
extern int8_t CAN_MSGLEN;

//define encoder
#define LENGTH_0_60 0
#define LENGTH_60 1
#define LENGTH_0 2
#define LENGTH_error 3


//define LV protocol 
////according to https://arrcrocket.org/T14#7868
#define MCDU 0x84
#define RQDU 0xC4
#define ACDU 0x44
#define CRMA 0xE4

#define ACKM 0x84
#define ACKA 0x44
#define ACKR 0xE4
#define DRPT 0x5B

#define MCDU_PRST 0xA0
#define ACDU_POND 0xDF
#define ACDU_POFD 0xAA
#define ACDU_P00D 0xB0
#define ACDU_P30D 0xB4
#define ACDU_P60D 0xB8
#define ACDU_P90D 0xBC

#define ACKM_OK 0x01
#define ACKM_INV 0xFD

#define ACKA_OK 0x01
#define ACKA_INV 0xFD

#define DRPT_P00D 0x30
#define DRPT_P30D 0x70
#define DRPT_P60D 0xB0
#define DRPT_P90D 0xF0
#define DRPT_STDB 0x07
#define DRPT_FTRN 0x0F

#include "CTinstruments/CTInstruments.h"

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* MAIN_LIB_H_ */