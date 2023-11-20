/*
 * CAN.h
 *
 * Created: 2019/7/5 下午 08:44:24
 *  Author: 王碩傑
 */ 


#ifndef CAN_H_
#define CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../CTinstruments/CTInstruments.h"

//========================================================


#define CAN_available() (CANSTMOB & ( 1 << RXOK))

void CAN_initialize(uint32_t);
uint8_t CANRead1byte(void);
void CANRead(void);//CANReading
void CANSend(uint32_t,int8_t);
void CANSETID(uint32_t);

#ifdef __cplusplus
}
#endif // extern "C"

#endif /* CAN_H_ */