/**
  ******************************************************************************
  * @file      drv_canopen.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-3-13
  * @brief     RSD79 board driver
  *            manage wheels drive canopen messages
  ******************************************************************************
  **/ 
  

/*
	syntron motor drive note:
	1. when enable drive firstly, should write 0x06,0x07,0x0f in turn to Index 0x6040.
       when running state, should write 0x07 to disable and 0x0f to enable drive.
	首次使能伺服驱动器，必须依次写入 0x06、0x07、0x0F。之后写 0x07
	则伺服断使能，再写 0x0F 则伺服使能；


*/


#include "./drv_canopen.h"
#include "../../User.stdlib/li.BSP/Inc/bsp_can2wheels.h"
#include "../../User/li.APP/util_time.h"


// syntron config commands
//uint8_t syntron_ControlMode[8] = {0x02,0x21,   20 };
//uint8_t syntron_DriveCanId[8] = {0x06,0x45,  0201};
//uint8_t syntron_CanBaudrate[8] = {0x07,0x45, 500};
 
// example canopen message(HEX)  602  2b 40 60 00| 06 00, 00 00  
const uint8_t enVoltage[8] = {0x2b,0x40,0x60,0x00, 0x06,0x00,0x00,0x00};
const uint8_t disOperation[8] = {0x2b,0x40,0x60,0x00, 0x07,0x00,0x00,0x00};
const uint8_t enOperation[8] = {0x2b,0x40,0x60,0x00, 0x0f,0x00,0x00,0x00};

uint8_t enDrive[8] = {0x2b,0x40,0x60,0x00, 0x06,0x00,0x00,0x00};

uint8_t OperationMode[8] = {0x2f,0x60,0x60,0x00,  0x03,0x00,0x00,0x00};
uint8_t PvAccTime[8] = {0x23,0x83,0x60,0x00, 0xe8,0x03,0x00,0x00};
uint8_t PvDecTime[8] = {0x23,0x84,0x60,0x00, 0xe8,0x03,0x00,0x00};
uint8_t PvTargetv[8] = {0x23,0xff,0x60,0x00, 0x64,0x00,0x00,0x00};
//////////////////////////////////////////////////////////////63

void can1open_ReadyAll( void ) 
{
  uint8_t tdata[2] = { 0x01,0x01 };
	can1wheels_SendMessage( tdata, 2, 0x000 );  util_waitms(6073); 
	tdata[1] = 0x02;
	can1wheels_SendMessage( tdata, 2, 0x000 );  util_waitms(6379); 
}


void can1open_EnableDrive1st( uint8_t drvid )
{
  uint8_t enDrive[8] = {0x2b,0x40,0x60,0x00, 0x06,0x00,0x00,0x00};
	//enDrive[4] = 0x06;
	can1wheels_SendMessage( enDrive, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(4973); 
  enDrive[4] = 0x07;
	can1wheels_SendMessage( enDrive, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(4573); 
  enDrive[4] = 0x0f;
	can1wheels_SendMessage( enDrive, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(4273); 
} 

void can1open_DisableDrive( uint8_t drvid )
{
  //enDrive[4] = 0x07;
	uint8_t enDrive[8] = {0x2b,0x40,0x60,0x00, 0x07,0x00,0x00,0x00};
	can1wheels_SendMessage( enDrive, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(439);  // 439
}
void can1open_EnableDrive( uint8_t drvid )
{
	//enDrive[4] = 0x0f;
	uint8_t enDrive[8] = {0x2b,0x40,0x60,0x00, 0x0f,0x00,0x00,0x00};
  can1wheels_SendMessage( enDrive, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(440);  // 440
}
//////////////////////////////////////////////////////////////63

void can1open_SetOperationMode( uint8_t drvid, uint8_t mode )
{
  uint8_t OperationMode[8] = {0x2f,0x60,0x60,0x00,  0x03,0x00,0x00,0x00};
	OperationMode[4] = mode;
	can1wheels_SendMessage( OperationMode, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(1911); 
}

void can1open_SetVelocity( uint8_t drvid, int32_t velocity )
{      // Velocity value to CANopen device is LSB first
  uint8_t PvTargetv[8] = {0x23,0xff,0x60,0x00, 0x64,0x00,0x00,0x00};
	PvTargetv[4] = (velocity &0x000000ff);
	PvTargetv[5] = (velocity &0x0000ff00)>>8;
	PvTargetv[6] = (velocity &0x00ff0000)>>16;
	PvTargetv[7] = (velocity &0xff000000)>>24;
	can1wheels_SendMessage( PvTargetv, 8, CANOPEN_COBID_SEND + drvid );  util_waitms(129);   // 137 43,37
}




/////////////////////////////////////////////////////////////////66 end of file


/*        


*/


	
	
	



