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
#define Maxlength 60
#define step_per_cm 50

void LVhandle(void);
void LVPosReset(uint8_t Addr);
void LVPosMax(uint8_t Addr);
void LVSetLength(uint8_t Addr, uint8_t angle); //set {LV} to {length} //no change in angle 'cause it's too time-wasting
void stepOutput(uint8_t num, bool output);
void enOutput(uint8_t num, bool output);
void dirOutput(uint8_t num, bool output);
int8_t ADDR2NUM(uint8_t ADDR);
uint8_t readLENGTH(uint8_t num);
int16_t LV_stepToGo[4] = {0, 0, 0, 0};
uint8_t LV_currentlength[4] = {0, 0, 0, 0};
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

/////////////////////////////////////////////////
///////////Things i need to modify///////////////
/////////////////////////////////////////////////

void CANhandle(void)
{
	if( CAN_available() )
	{
		CANRead();
		if (CAN_MSG[0] == 0) //length set to what we enter
		{
			if (CAN_MSG[1] == 0 && LV_POS_RST_flag[0] == 0 && done == true)
			{
				LV_currentlength[0] = CAN_MSG[1];
				LVPosReset(0x01);
				done = false;
			}
			else if (CAN_MSG[1] == Maxlength && LV_POS_RST_flag[0] == 0 && done == true)
			{
				LV_currentlength[0] = CAN_MSG[1];
				LVPosMax(0x01);
				done = false;
			}		
			else
			{
				CAN_MSG[1] = (CAN_MSG[1] % 16) + ((CAN_MSG[1] / 16) % 16) * 10;
				LVSetLength(0x01,CAN_MSG[1]);
				LV_currentlength[0] = CAN_MSG[1];
				done = false;
			}
		}
		else if (CAN_MSG[0] == 1) //decrease the length we enter
		{
			if (CAN_MSG[1] == 0)
			{
				
			}
			else if (CAN_MSG[1] != 0 && LV_POS_RST_flag[0] == 0 && done == true)
			{
				CAN_MSG[1] = (CAN_MSG[1] % 16) + ((CAN_MSG[1] / 16) % 16) * 10;
				LVSetLength(0x01, LV_currentlength[0] - CAN_MSG[1]);
				LV_currentlength[0] = LV_currentlength[0] - CAN_MSG[1];
				done = false;
			}
		}
		else if (CAN_MSG[0] == 2) //increase the length we enter
		{
			if (CAN_MSG[1] == 0)
			{
				
			}
			else if (CAN_MSG[1] != 0 && LV_POS_RST_flag[0] == 0 && done == true)
			{
				CAN_MSG[1] = (CAN_MSG[1] % 16) + ((CAN_MSG[1] / 16) % 16) * 10;
				LVSetLength(0x01, LV_currentlength[0] + CAN_MSG[1]);
				LV_currentlength[0] = LV_currentlength[0] + CAN_MSG[1];
				done = false;
			}
		}
	} //CAN available
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

void LVPosMax(uint8_t Addr)
{
	switch (Addr)
	{
		case LV_1_ADDR:
			LV_POS_RST_flag[0] = 2;
			break;
		case LV_2_ADDR:
			LV_POS_RST_flag[1] = 2;
			break;
		case LV_3_ADDR:
			LV_POS_RST_flag[2] = 2;
			break;
		case LV_4_ADDR:
			LV_POS_RST_flag[3] = 2;
			break;
	}
}

void LVSetLength(uint8_t Addr, uint8_t length) //set {LV} to {degree} //change LVSetDegree to sth else to make it more clear
{
	switch (Addr)
	{
		case LV_1_ADDR:
			LV_stepToGo[0] = (length - LV_currentlength[0]) * step_per_cm ; //  200 steps per 4 cm // 50 steps per 1 cm
			break;
		case LV_2_ADDR:
			LV_stepToGo[0] = (length - LV_currentlength[0]) * step_per_cm ; //  200 steps per 4 cm // 50 steps per 1 cm
			break;
		case LV_3_ADDR:
			LV_stepToGo[0] = (length - LV_currentlength[0]) * step_per_cm ; //  200 steps per 4 cm // 50 steps per 1 cm
			break;
		case LV_4_ADDR:
			LV_stepToGo[0] = (length - LV_currentlength[0]) * step_per_cm ; //  200 steps per 4 cm // 50 steps per 1 cm
			break;
	}
}

/////////////////////////////////////////////////
/////////////Don't give it a shxt////////////////
/////////////////////////////////////////////////

void LVhandle(void)
{
	for(int i = 0; i<4; i++)	//pos reset handle
	{
		switch (LV_POS_RST_flag[i])
		{
			case 0:									//normally work
				break;
			case 1:									//PRST CMD is sent, check current POS
				if (readLENGTH(i) != LENGTH_0)		    //stick touch the start point
				{
					LV_POS_RST_flag[i] = 3;
				}
				else
				{
					LV_POS_RST_flag[i] = 5;
				} 
				break;
			case 2:
				if (readLENGTH(i) != LENGTH_Max)
				{
					LV_POS_RST_flag[i] = 4;
				}
				else
				{
					LV_POS_RST_flag[i] = 5;
				}
				break;
			case 3:
				LV_stepToGo[i] = -step_per_cm;				//walk -1 cm until touch the sensor A
				if(readLENGTH(i) == LENGTH_0)
				{ 
					LV_stepToGo[i] = 0;
					LV_POS_RST_flag[i] = 5;
				}
				break;
			case 4:
				LV_stepToGo[i] = step_per_cm;					//walk 1 cm until touch the sensor B
				if(readLENGTH(i) == LENGTH_Max)
				{
					LV_stepToGo[i] = 0;
					LV_POS_RST_flag[i] = 5;
				}
				break;
			case 5:									//reset pos done
				LV_state[i] = state_FTRN;
				LV_POS_RST_flag[i] = 0;
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
uint8_t readLENGTH(uint8_t num)
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
		if (B)  //AB=11 //something wrong
			return LENGTH_error;
		else  //AB=10 only A get the signal //stick touch the start point
			return LENGTH_0;
	}
	else
	{
		if (B)  //AB=01 only B get the signal //stick touch the end point
			return LENGTH_Max;
		else  //AB=00 //normally work
			return LENGTH_0_Max;
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

