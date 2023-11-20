/*
 * CAN.c
 *
 * Created: 2019/7/5 下午 08:44:41
 *  Author: 王碩傑
 */ 

#include "../inc/CAN.h"

uint32_t CANRXID = 0x000;
//b00000000000
uint32_t CANTXID = 0x400;
//b10000000000

uint8_t CAN_MSG[8] = {0,0,0,0,0,0,0,0};
uint8_t CAN_DATATOSEND[8] = {0,0,0,0,0,0,0,0};
int8_t CAN_MSGLEN = 1;	

void CAN_initialize(uint32_t RXID)
{
	CANRXID = RXID;
	//CANTXID = TXID;
	CANGCON = ( 1 << SWRES ); 
	for ( int8_t mob=0; mob<6; mob++ ) {

		CANPAGE = ( mob << 4 );     	// Selects Message Object 0-5

		CANCDMOB = 0x00;       		// Disable mob

		CANSTMOB = 0x00;     		// Clear mob status register;
	}
	
	CANGCON &= 0;//CAN General Control Register
	CANGCON |= (1<<ENASTB);//enter enable mode
	//CANGCON |= (1<<TTC); //enter TTC Mode to send just one frame
	CANPAGE = 0x00;//CAN Page Mobs Register
	CANPAGE &= ~(1<<AINC);
	//AINC: auto increase of CAN data buffer index(0)
	//MObNB Mob num = 0 (0000)??????
	//CANCDMOB &= 0;//CAN MOb Control and DLC Register
	
	CANCDMOB = (1<<CONMOB1|0<<CONMOB0|0<<RPLV|0<<IDE|1<<DLC3|0<<DLC2|0<<DLC1|0<<DLC0);
	//CONMOB Config of MOb: En Reception(10)
	//RPLV dis Auto reply(0)
	//IDE: CAN 2.0A (11bits ID)(0)
	//DLC Data Len 8Byte(1000)
	//CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 1 << ENTX );   // Enable interrupts on receive
	//CANIE2 |= (1<<IEMOB1);//en interrupt on mob1(for tx)
	CANTCON = 0x00;
	//CAN BT 查表500kbps
	CANBT1 = 0x02;//CAN bit timing Register 1
	//00000010
	//BRP[5:0] 0x1E Baud Rate Psc = 15
	CANBT2 = 0x0C;//CAN bit timing Register 2
	//00000100 //Same as the example on AVR Freaks
	//SJW[1:0] Re-Synchronization Jump Width = 0(00)
	//PRS[2:0] Propagation Time Segment = 2(010) //used to compenstate physical delay times
	CANBT3 = 0x37;//CAN bit timing Register 3
	//00010011 //Same as the example on AVR Freaks
	//PHS[22:20] Phase Segment 2 (001) (<=PHS1) //used to conpenstate phase edge errors
	//PHS[12:10] Phase Segment 1 (001)
	//SMP Sample point (1) //used to filtered possible noise on TxCAN(Vote of 3 points)
	CANIDM = 0xffffffff;//CAN ID Mask Register Mask everything(32bit all 1)
	//CANIDT &= 0x00000000;//CAN ID Tag Register(32bit all 0)
	CANSETID(CANRXID);
	//RTRTAG Remote Tx Request(0)
	//RB0TAG Reserved Bit TAG(0)
}

void CANSETID(uint32_t ID)
{
	CANIDT = 0x00000000;
	CANIDT |= (ID<<21);
}

uint8_t CANRead1byte()
{
	uint8_t temp = CANMSG;
	CANMSG=0x00;
	CANCDMOB |= (1<<CONMOB1|1<<CONMOB0);
	//delay(1);
	CANMSG=0x00;
	CANSTMOB &= ~(1 << RXOK);
	return temp;
}

void CANRead()
{
	CAN_MSGLEN = ( CANCDMOB & 0x0F );	// DLC, number of bytes to be received
	CANPAGE &= ~(1<<0);//INDEX to 0
	CANPAGE &= ~(1<<1);
	CANPAGE &= ~(1<<2);
	for ( int8_t i = 0; i < CAN_MSGLEN; i++ ){
		CAN_MSG[i] = CANMSG; 		// Get data, INDX auto increments CANMSG
	}

	CANCDMOB = (( 1 << CONMOB1 ) | (0<<CONMOB0));  // Enable Reception 11 bit IDE DLC8
	CANMSG=0x00;
	CANSTMOB &= ~(1 << RXOK);
	// Note - the DLC field of the CANCDMO register is updated by the received MOb. If the value differs from expected DLC, an error is set

}

void CANSend(uint32_t TXID,int8_t len)
{
	CANTXID = TXID;
	CANPAGE = (1<<MOBNB0);
	while ( CANEN2 & (1<<ENMOB1)); // Wait for MOb 1 to be free
	CANSTMOB = 0x00;
	CANSETID(CANTXID);
	CANIDM = 0xffffffff;
	for(int i=0;i<len;i++)
	{
		CANMSG = CAN_DATATOSEND[i];
	}
	CANCDMOB =((0 << CONMOB1) |( 1 << CONMOB0 ) | ( 0 << IDE ) | len);
	while (  !( CANSTMOB & ( 1 << TXOK ) ) );
	CANCDMOB = 0x00;	// Disable Transmission
	CANSTMOB = 0x00;	// Clear TXOK flag
	CANPAGE = 0x00;
}