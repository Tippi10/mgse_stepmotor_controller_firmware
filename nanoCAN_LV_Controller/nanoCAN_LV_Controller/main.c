/*
 * nanoCAN_LV_Controller.c
 *
 * Created: 7/6/2019 6:22:14 PM
 * Author : thomasya
 */ 

#include "main_lib.h"
#define Atmega64M1


/////////////////////////////////////////////////
/////////// CAN variables & functions////////////
/////////////////////////////////////////////////
#define CAN_SEND_LEN 3
#define CATCAN_ID 0x300
void CANhandle(void);


/////////////////////////////////////////////////
///////////// LV variables & functions///////////
/////////////////////////////////////////////////
#define LV_1_ADDR 0x01
#define LV_2_ADDR 0x02
#define LV_3_ADDR 0x03
#define LV_4_ADDR 0x04

void LVhandle(void);
void LVPosReset(uint8_t Addr);
void LVSetDegree(uint8_t Addr, uint8_t angle); //set {LV} to {degree}
void stepOutput(uint8_t num, bool output);
void enOutput(uint8_t num, bool output);
void dirOutput(uint8_t num, bool output);
int8_t ADDR2NUM(uint8_t ADDR);
uint8_t readOPB(uint8_t num);
int16_t LV_stepToGo[4] = {0, 0, 0, 0};
uint8_t LV_currentAngle[4] = {0, 0, 0, 0};
bool LV_HL = true; //step high or low

bool done = true;


/////////////////////////////////////////////////
////// State Machine variables & functions //////
/////////////////////////////////////////////////
#define state_STDB 0xAA
#define state_FTRN 0xBB
uint8_t LV_state[4] = {state_STDB, state_STDB, state_STDB, state_STDB};
uint8_t LV_POS_RST_flag[4] = {0, 0, 0, 0};


/////////////////////////////////////////////////
///////////////////// Setup /////////////////////
/////////////////////////////////////////////////
void setup()
{
	CAN_initialize(0x000);
	GPIO_HIGH(V12_ctrl);
	LVPosReset(0x01);
}

void loop()
{
	
	wdt_reset();
	
	CANhandle();
	//LV_stepToGo[0] = 100;
	LVhandle();
	
	
}


void GPIO_select()
{
	GPIO_MODE_OUTPUT(LED);
	
	GPIO_MODE_OUTPUT(stepPin1);
	GPIO_MODE_OUTPUT(dirPin1);
	GPIO_MODE_OUTPUT(enPin1);
	GPIO_MODE_OUTPUT(stepPin2);
	GPIO_MODE_OUTPUT(dirPin2);
	GPIO_MODE_OUTPUT(enPin2);
	GPIO_MODE_OUTPUT(stepPin3);
	GPIO_MODE_OUTPUT(dirPin3);
	GPIO_MODE_OUTPUT(enPin3);
	GPIO_MODE_OUTPUT(stepPin4);
	GPIO_MODE_OUTPUT(dirPin4);
	GPIO_MODE_OUTPUT(enPin4);
	
	GPIO_MODE_OUTPUT(V12_ctrl);
	
	GPIO_MODE_OUTPUT(multA);
	GPIO_MODE_OUTPUT(multB);
	GPIO_MODE_INPUT(phaseA);
	GPIO_MODE_INPUT(phaseB);
}
void Error_Handler() //Do while WDT_VECT triggered
{

}
void CANhandle(void)
{
	if( CAN_available() )
	{
		CANRead();
		if (CAN_MSG[0] == CAN_MSG[1])
		{
			if (CAN_MSG[1] == CAN_MSG[2])
			{
				
				if(CAN_MSG[0] == 0xFF)
				{
					CAN_MSG[0] = 0;
					LV_currentAngle[0] = CAN_MSG[0];
					LVPosReset(0x01);
				}
				
				if (LV_POS_RST_flag[0] == 0 && done == true)
				{
					LVSetDegree(0x01,CAN_MSG[0]);
					LV_currentAngle[0] = CAN_MSG[0];
					done = false;
				}
			}
		}
	} //CAN available
}

void LVReset(uint8_t Addr)
{
	LVPosReset(Addr);
	LVSetDegree(Addr, 0); // Move LV to the origin (assuming 0 degrees is the origin)
}

void LVMoveForward5cm(uint8_t Addr)
{
	// One revolution = 200 steps = 10 degrees = 4cm, so 5cm is 200 / 4 * 5 = 50 steps
	// Use the LVSetDegree function, calculate the required angle, and then set LV's angle
	uint8_t currentAngle = LV_currentAngle[ADDR2NUM(Addr)];
	LVSetDegree(Addr, currentAngle + 50); // Move LV forward by 5cm
}

void LVMoveBackward5cm(uint8_t Addr)
{
	uint8_t currentAngle = LV_currentAngle[ADDR2NUM(Addr)];
	LVSetDegree(Addr, currentAngle - 50); // Move LV backward by 5cm
}


void LVPosReset(uint8_t Addr)
{
	switch (Addr)
	{
		case LV_1_ADDR:
			LV_POS_RST_flag[0] = 1;
			break;
		case LV_2_ADDR:
			LV_POS_RST_flag[1] = 1;
			break;
		case LV_3_ADDR:
			LV_POS_RST_flag[2] = 1;
			break;
		case LV_4_ADDR:
			LV_POS_RST_flag[3] = 1;
			break;
	}
}
void LVSetDegree(uint8_t Addr, uint8_t angle) //set {LV} to {degree}
{
	switch (Addr)
	{
		case LV_1_ADDR:
			LV_stepToGo[0] = (angle - LV_currentAngle[0]) * 20; //  64*200/360 = 35.55555555555
			break;
		case LV_2_ADDR:
			LV_stepToGo[1] = (angle - LV_currentAngle[1]) * 20; //  64*200/360 = 35.55555555555
			break;
		case LV_3_ADDR:
			LV_stepToGo[2] = (angle - LV_currentAngle[2]) * 20; //  64*200/360 = 35.55555555555
			break;
		case LV_4_ADDR:
			LV_stepToGo[3] = (angle - LV_currentAngle[3]) * 20; //  64*200/360 = 35.55555555555
			break;
	}
	
}
void LVhandle(void)
{
	for(int i = 0; i<4; i++)	//pos reset handle
	{
		switch (LV_POS_RST_flag[i])
		{
			case 0:									//no need to reset
				break;
			case 1:									//PRST CMD is sent, check current POS
				if (readOPB(i) == PLATE_90_360)		//angle = 90~360
					LV_POS_RST_flag[i] = 2;
				else								//angle = 0~90
					LV_POS_RST_flag[i] = 3;
				break;
			case 2:
				LV_stepToGo[i] = 35;				//rotates 1 degree toward 360
				if(readOPB(i) != PLATE_90_360)
				{
					LV_stepToGo[i] = 0;
					LV_POS_RST_flag[i] = 4;
				}
				break;
			case 3:
				LV_stepToGo[i] = -35;					//rotates 1 degree toward 0
				if(readOPB(i) == PLATE_90_360)
				{
					LV_stepToGo[i] = 0;
					LV_POS_RST_flag[i] = 4;
				}
				break;
			case 4:									//reset pos done
				LV_state[i] = state_FTRN;
				LV_POS_RST_flag[i] = 0;
				LV_currentAngle[i] = 0;
				break;
		}
		
	}

	if (_Control_Flag_0 >= 5 ) //about 28 degree per sec
	{
		if (LV_HL) //
		{
			for(int i = 0; i<4; i++)
			{
				if (LV_stepToGo[i]>0)
				{
					enOutput(i, false);
					dirOutput(i, false);
					stepOutput(i, true);
					LV_stepToGo[i]--;
				}
				else if (LV_stepToGo[i]<0)
				{
					enOutput(i, false);
					dirOutput(i, true);
					stepOutput(i, true);
					LV_stepToGo[i]++;
				}
				else //LV_stepToGo[i] = 0
				{
					enOutput(i, true);
					if(i == 0)
						done = true;
				}
			}
			CCF0();
			LV_HL = false;
		}
		else
		{
			for(int i = 0; i<4; i++)
			{
				stepOutput(i, false);
			}
			CCF0();
			LV_HL = true;
		}
	}

}
uint8_t readOPB(uint8_t num)
{
	switch(num)
	{
		case 0:
		GPIO_LOW(multA);
		GPIO_LOW(multB);
		break;
		case 1:
		GPIO_HIGH(multA);
		GPIO_LOW(multB);
		break;
		case 2:
		GPIO_LOW(multA);
		GPIO_HIGH(multB);
		break;
		case 3:
		GPIO_HIGH(multA);
		GPIO_HIGH(multB);
		break;
	}
	bool A = GPIO_DREAD(phaseA);
	bool B = GPIO_DREAD(phaseB);
	if (A)
	{
		if (B)  //30~60
			return PLATE_30_60;
		else  //0~30
			return PLATE_0_30;
	}
	else
	{
		if (B)  //60~90
			return PLATE_60_90;
		else  //90~360
			return PLATE_90_360;
	}

}
void stepOutput(uint8_t num, bool output)
{
	if(output)
	{
		switch (num)
		{
			case 0:
			GPIO_HIGH(stepPin1);
			break;
			case 1:
			GPIO_HIGH(stepPin2);
			break;
			case 2:
			GPIO_HIGH(stepPin3);
			break;
			case 3:
			GPIO_HIGH(stepPin4);
			break;
		}
	}
	else
	{
		switch (num)
		{
			case 0:
			GPIO_LOW(stepPin1);
			break;
			case 1:
			GPIO_LOW(stepPin2);
			break;
			case 2:
			GPIO_LOW(stepPin3);
			break;
			case 3:
			GPIO_LOW(stepPin4);
			break;
		}
	}
}
void enOutput(uint8_t num, bool output)
{
	if(output)
	{
		switch (num)
		{
			case 0:
			GPIO_HIGH(enPin1);
			break;
			case 1:
			GPIO_HIGH(enPin2);
			break;
			case 2:
			GPIO_HIGH(enPin3);
			break;
			case 3:
			GPIO_HIGH(enPin4);
			break;
		}
	}
	else
	{
		switch (num)
		{
			case 0:
			GPIO_LOW(enPin1);
			break;
			case 1:
			GPIO_LOW(enPin2);
			break;
			case 2:
			GPIO_LOW(enPin3);
			break;
			case 3:
			GPIO_LOW(enPin4);
			break;
		}
	}
}
void dirOutput(uint8_t num, bool output)
{
	if(output)
	{
		switch (num)
		{
			case 0:
			GPIO_HIGH(dirPin1);
			break;
			case 1:
			GPIO_HIGH(dirPin2);
			break;
			case 2:
			GPIO_HIGH(dirPin3);
			break;
			case 3:
			GPIO_HIGH(dirPin4);
			break;
		}
	}
	else
	{
		switch (num)
		{
			case 0:
			GPIO_LOW(dirPin1);
			break;
			case 1:
			GPIO_LOW(dirPin2);
			break;
			case 2:
			GPIO_LOW(dirPin3);
			break;
			case 3:
			GPIO_LOW(dirPin4);
			break;
		}
	}
}

int8_t ADDR2NUM(uint8_t ADDR)
{
	switch (ADDR)
	{
		case LV_1_ADDR:
			return 0;
		case LV_2_ADDR:
			return 1;
		case LV_3_ADDR:
			return 2;
		case LV_4_ADDR:
			return 3;
		default:
			return -1;
	}
	
}

