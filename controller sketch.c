#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

// may have lib already
void CAN_available(); //return data size in CAN bufffer (bytes)
void CANRead1byte(); //read one byte in CAN buffer
void micros();


void LVRotateDegree(int8_t degree); //rotate LV for {degree} degrees (-90~90 degree)
void LVPWM(uint32_t microseconds, uint16_t freq); //PWM output for LV. Output a PWM signal in frequency = {freq}, 50% duty cycle, for {microseconds} microseconds


#define stepPin = PD4;
#define dirPin = PD6;
#define enPin = PD5;

void main()
{
	uint8_t CAN_MSG = 0; //message received by CAN
	int8_t valveDegree = 0; //current LV angle (0~90degree) 
	int8_t DegreeCMD = 0; //command of next desired LV angle((0~90degree)

	/*
		CANsetup();
		pinMode(stepPin,OUTPUT);
		pinMode(dirPin,OUTPUT);
		pinMode(enPin,OUTPUT);
	*/

	while (1)
	{
		if (CAN_available())
		{
			CAN_MSG = CANRead1byte();
		}
		if (CAN_MSG == 255)
			DegreeCMD = 90;
		else if(CAN_MSG == 0)
			DegreeCMD = 0;


		if (DegreeCMD != valveDegree)
		{
			LVRotateDegree(DegreeCMD - valveDegree);
			valveDegree = DegreeCMD;
		}

	}
}


void CAN_available() //return data size in CAN bufffer (bytes)
{
	/*
	unit8_t bytes_of_data = 0;

	bytes_of_data = dataInCANBuffer();

	return bytes_of_data;
	*/
}


void CANRead1byte() //read one byte in CAN buffer
{
	uint8_t data = 0;
	//data = CANread()

	return data;
}

void LVRotateDegree(uint8_t degree) //rotate LV for degree (-90~90 degree)
{
	//digitalWrite(enPin, LOW);
	if (degree > 0)
		//digitalWrite(dirPin, HIGH);
	else
		//digitalWrite(dirPin, LOW);
	uint16_t PWMfreq = 64 * 200 * 20 / 360; //PWM output freq (for 20degree per sec)
	LVPWM((uint32_t)degree * 1000 / 20, PWMfreq);
	//digitalWrite(enPin, HIGH);
}


void LVPWM(uint32_t microseconds, uint16_t freq) //PWM output for LV. Output a PWM signal in frequency = {freq}, 50% duty cycle, for {microseconds} microseconds
{
	uint32_t startTime = micros();
	while ((startTime + microseconds) > micros())
	{
		//PWMout(stepPin, freq, 0.5) //pin, frequency, duty cycle
	}
}

