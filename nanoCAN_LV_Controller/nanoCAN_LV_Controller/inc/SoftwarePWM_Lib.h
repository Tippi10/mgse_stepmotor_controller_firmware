/*
 * SoftwarePWM_Lib.h
 *
 * Created: 2019/4/8 上午 01:40:04
 *  Author: 王碩傑
 */ 

/*
Pin Mapping Need to change GPIO_Map as Well
0:pb2
1:pc4
2:pc5
3:pc6
4:pd6
5:pb4
6:pc7
7:pd7
*/


#ifndef SoftwarePWM_LIB_H_
#define SoftwarePWM_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

//Software PWM========================================================

//define common Software PWM arguments
//for bit masking
#define PWM0 0
#define PWM1 1
#define PWM2 2
#define PWM3 3
#define PWM4 4
#define PWM5 5
#define PWM6 6
#define PWM7 7

#define PWM_Pin0 _PD6
#define PWM_Pin1 _PD7
#define PWM_Pin2 _PB2
#define PWM_Pin3 _PC4
#define PWM_Pin4 _PC5
#define PWM_Pin5 _PC6
#define PWM_Pin6 _PB4
#define PWM_Pin7 _PC7

//define Software PWM usable function
void SPWM_init(void);
#define _SPWM_Attach(DDRX,PORT,PIN,MSK,PWM) (PWM_Mask |= _BV(PWM))
#define SPWM_Attach(PXX) _SPWM_Attach(PXX)
#define _SPWM_Detach(DDRX,PORT,PIN,MSK,PWM) (PWM_Mask &= ~_BV(PWM))
#define SPWM_Detach(PXX) _SPWM_Detach(PXX)
#define _SPWM_Write(DDRX,PORT,PIN,MSK,PWM,val) PWMValue[PWM] = val << 1;
#define SPWM_Write(PXX,val) _SPWM_Write(PXX,val)
#define SPWM_freq(freq) PWMPeriod = (int)(20000.0/freq)
#define SPWM_freq2ms(freq) (int)(20000.0/freq) 

//========================================================

#include "../CTinstruments/CTInstruments.h"
	
#ifdef __cplusplus
}
#endif // extern "C"

#endif /* SoftwarePWM_LIB_H_ */


/*
Chosing pin temp

32 CLK0
1 MISO OC1A
2 MOSI OC0A Tx
3 OC1B
6 CanTx
7 RXCAN
X8 SPIMISO

27
X28 SPISCK
29
30

9 SPI MOSI
12 ISPSCK
13
14 pd6
15 pd7
16 pb2
17 pc4

18 pc5
22 pc6
23 pb3//used in FM
24 pb4
25 pc7
26 pb5//used in FM
21 AREF


*/