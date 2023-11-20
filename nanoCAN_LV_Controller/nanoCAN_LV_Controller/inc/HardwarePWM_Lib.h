/*
 * HardwarePWM_Lib.h
 *
 * Created: 2019/4/8 上午 01:40:04
 *  Author: 王碩傑
 */ 


#ifndef HardwarePWM_LIB_H_
#define HardwarePWM_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

//Hardware PWM========================================================
//define common Hardware PWM arguments
#define HPWM_f_100 10000
//freq 100hz
#define HPWM_f_Norm 0
//normal pwm

//define Hardware PWM usable function
void HardwarePWM_init(unsigned int,unsigned int);
#define HPWM_WriteA(PWM_Dutycycle) OCR1A = PWM_Dutycycle
#define HPWM_WriteB(PWM_Dutycycle) OCR1B = PWM_Dutycycle

//========================================================
	
#include "../CTinstruments/CTInstruments.h"
	
#ifdef __cplusplus
}
#endif // extern "C"

#endif /* HardwarePWM_LIB_H_ */