/**
  ******************************************************************************
  * @file      drv_canopen.h
  * @author    liyc
  * @version   V1.0
  * @date      2024-3-13
  * @brief     RSD79 board driver
  *            manage wheels drive canopen messages
  ******************************************************************************
  **/ 


#ifndef __DRV_CANOPEN_H
#define	__DRV_CANOPEN_H


#include <stdint.h>
//#include  "../li.uCOS/uCOS_bsp.h"
//#include "stm32f4xx.h"

// CAUTION: in CANopen, index/subindex/data are LSB sent firstly.
//////////////////////////27

// SDO message to drive node
// cob-id
#define  CANOPEN_COBID_SEND     (0x600)
// command and length
#define  CANOPEN_CMD_WR4B     (0x23)  
#define  CANOPEN_CMD_WR2B     (0x2b)  
#define  CANOPEN_CMD_WR1B     (0x2f)  
#define  CANOPEN_CMD_RD     (0x40)  

// SDO message from drive node
// cob-id
#define  CANOPEN_COBID_RECEIVE     (0x580)
// reply and length
#define  CANOPEN_REP_WR     (0x60)  
#define  CANOPEN_REP_RD4B     (0x43)  
#define  CANOPEN_REP_RD2B     (0x4b)  
#define  CANOPEN_REP_RD1B     (0x4f)  
#define  CANOPEN_REP_ERROR     (0x80)  

// RPDO  send to node some run parameters in short frame 
// TPDO  node send some status data in short frame

// index write/read
#define  CANOPEN_INDEX_CONTROLWORD     (0x6040)
#define  CANOPEN_INDEX_OPMODE        (0x6060)
#define  CANOPEN_INDEX_TARGETVELOCITY        (0x60ff)
//#define  CANOPEN_INDEX_TARGETVELOCITYN        (0x60ff)
#define  CANOPEN_INDEX_PROFACC        (0x6083)
#define  CANOPEN_INDEX_PROFDEC        (0x6084)

// index read only
#define  CANOPEN_INDEX_ERRORCODE     (0x603f)
#define  CANOPEN_INDEX_STATUSWORD     (0x6041)
#define  CANOPEN_INDEX_READOPMODE        (0x6061)


/////////////////////////////////////////////////////////58

void can1open_ReadyAll( void ); 
void can1open_EnableDrive1st( uint8_t drvid );
void can1open_DisableDrive( uint8_t drvid );
void can1open_EnableDrive( uint8_t drvid );
void can1open_SetOperationMode( uint8_t drvid, uint8_t mode );
void can1open_SetVelocity( uint8_t drvid, int32_t velocity );









#endif   /* __DRV_CANOPEN_H */


/////////////////////////////////////////////////////////////////66 end of file




