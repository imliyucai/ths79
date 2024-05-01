/**
  ******************************************************************************
  * @file      task_can1wheels.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-2-3
  * @brief     RSD79 board app task
	*            handle the comm with wheels drive CAN port
  ******************************************************************************
  **/ 
	

#include "./tasks.h"
#include "../li.BSP/Inc/bsp_can2wheels.h"
#include "../li.DRIVER/drv_canopen.h"

#include "./cmds.h"
#include "../li.BSP/Inc/bsp_iic.h"
#include "../li.BSP/Inc/bsp_timer.h"


extern  OS_TMR  WheelsTimer;

//#define  TASK_CAN1WHEELS_STK_SIZE      128 
OS_TCB  taskCan1wheelsTCB;
CPU_STK  taskCan1wheelsStk[ TASK_CAN1WHEELS_STK_SIZE ];

extern  OS_TCB  taskCan1wheelsTCB; 


////////////////////////////////////37 the task function
void task_Can1wheels( void* p_arg )
{
	(void)p_arg;
	
	OS_ERR   err;
	CPU_SR_ALLOC();   // for OS_CRITICAL_ENTER
	
	int k=0;
	
	//uint8_t dat[8] = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08 };
  uint8_t rdat[8];
	
	uint8_t  len;
	uint16_t  id;
	
	uint8_t  canx[17] ={ 0x99,   0x0B,0x20,0x43,0x06,0x01,0x00,0x00,0x02,0x01, 0xc2,0xc1,0xdd,   0xe9,0xf9 };
	uint8_t  backmsg[13] ={ 0x99, 0xcc };
	
	uint32_t TxMailbox;

	OS_FLAGS  flags_rdy;
	OS_MSG_SIZE  msg_size;
	uint8_t   *pMsg;
	
	
	
	
		
	
	
  can1wheels_Init( CAN1BAUDRATE500K );
  //printf( " return %d, from HAL_CAN_Init(). \n", rethal );

	iic2xl9555_Output( 0, 0x00 );  // NOTE: fr_signal should set to LOW
  wheels_Powerup();     // must power on wheels controller for can communication
	
  // ready to run wheels
	can1open_ReadyAll();     // wait long time.......
	
	can1open_EnableDrive1st( 1 );   
	can1open_EnableDrive1st( 2 );   
		
	can1open_SetOperationMode( 1, 3 );  // PV mode
  can1open_SetOperationMode( 2, 3 );  // PV mode
	 
	can1open_SetVelocity( 1, 0 );      // zero velocity first
	can1open_SetVelocity( 2, 0 );      // zero velocity first
	 
	can1open_EnableDrive( 1 );
	can1open_EnableDrive( 2 );
	 
	timer6_Config();
	
////////////////17 processing all cmd to wheels drive 	
	while( DEF_TRUE ) 
	{  

		OS_CRITICAL_ENTER();            // should not crack in send data duration
    
    // wait Q message from task_Dispatchcmd or CAN1_rx isr
    pMsg = OSTaskQPend(  (OS_TICK      ) 303,                      // wait Q... forever, ??
		                     (OS_OPT       ) OS_OPT_PEND_BLOCKING,   // blocking here 
		                     (OS_MSG_SIZE *)&msg_size,
		                     (CPU_TS      *) 0,                      // need time stamp of the Q ?
									       (OS_ERR      *)&err  );                 // return error 
		
		// 当超过 300ms 没有收到给速命令时，固件程序应该立刻刹停机器。 
		/* if( err == OS_ERR_TIMEOUT )
    {    // why not go into here?
		  //can1open_SetVelocity( 1, 0x0 );   // LSB first, left wheel
			//can1open_SetVelocity( 2, 0x0 );
		}   */
		
		if( err == OS_ERR_NONE )
		{
		  if( pMsg != NULL )    // need must?
			{
				for( int i=0; i<msg_size; i++ ) {  canx[i] = pMsg[i];   }    // copy data
				
//////////////////////23 Q message from drive, maybe alarm frame 				
				// this Q from CAN1
				if( canx[0] == 0x99 && canx[msg_size-2] == 0xe9 && canx[msg_size-1] == 0xf9 )
				{
				  if( canx[9] == CAN1_BACK_FLAG )   // a valid message from CAN1
					{
					  
					}
				}
				
				
//////////////////////23 Q message from task_uart2host, should be running cmd 					
				else    // this Q from task_Dispatchcmd, maybe cmd to drive wheels
				{
				  switch( canx[0] )   // 当超过300ms没有收到该命令时，固件程序应该立刻刹停机器人。
					{
					  case HOSTCMD_SETWHEELSVELOCITY10:    // to set interrupt timer?
							can1open_SetVelocity( 1, (int32_t)(int16_t)((canx[1]<<8) + canx[2]) );   // LSB first, left wheel
			 	      can1open_SetVelocity( 2, (int32_t)(int16_t)((canx[3]<<8) + canx[4]) );   // LSB first, right wheel
						  /////***** in host cmd, MSB first, while in CAN message, LSB first. !!!!
						
						  timer6_Reset();   // the 300ms timer
						  //OSTmrStop( &WheelsTimer, OS_OPT_TMR_NONE, 0, &err );
              //OSTmrStart( &WheelsTimer, &err );
						
						  backmsg[1] =5;  backmsg[2] =HOSTCMD_SETWHEELSVELOCITY10;	backmsg[3]=0x00;        
				      backmsg[4]=0xe9; backmsg[5]=0xf9;
				       // back message send to task_uart2host
				      OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		                        (void      *)backmsg, 
		                        (OS_MSG_SIZE) (5+1), 
		                        (OS_OPT     ) OS_OPT_POST_FIFO,
		                        (OS_ERR    *)&err  );						
						
						  break;
						
						default:  break;
					}
					
					
				}
				
		
			}
		
		}    


////////////////////////////29 some status (maybe from keyin) should be processing by drive		
////////// check FLAG_GRP status bits, and process the specified status
    flags_rdy = OSFlagPend(  (OS_FLAG_GRP *)&AppSysStatus,
                             (OS_FLAGS     )( 0x0000af00 ),
                             (OS_TICK      )0,         // when any flags are 1 to...
                             (OS_OPT       ) OS_OPT_PEND_NON_BLOCKING +OS_OPT_PEND_FLAG_SET_ANY +OS_OPT_PEND_FLAG_CONSUME,
                             (CPU_TS      *)0,
                             (OS_ERR      *)&err  );  
		
		if( flags_rdy &0x00008800 )
		{
		  
			can1open_SetVelocity( 1, 0x0 );   // LSB first, left wheel
			can1open_SetVelocity( 2, 0x0 );
			
			can1open_DisableDrive( 1 );
			can1open_DisableDrive( 2 );
		}
		else if( flags_rdy &0x00002200 )
		{
		  can1open_EnableDrive( 1 );
			can1open_EnableDrive( 2 );
		}    
		

			
		OS_CRITICAL_EXIT();             // back from critical region
	//	OSTimeDlyHMSM( 0, 0, 29, 0, OS_OPT_TIME_DLY, & err );  
	}
	
}

void timer_wheels_callback( void )
{
  //can1open_SetVelocity( 1, 0x0 );   // LSB first, left wheel
	//can1open_SetVelocity( 2, 0x0 );
}


/*
//	  can1wheels_SendMessageX( pMsg, pMsg[9], (pMsg[10]<<8)|pMsg[11] );
				
				OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		              (void      *) canx, 
		              (OS_MSG_SIZE) 15, 
		              (OS_OPT     ) OS_OPT_POST_FIFO,
		              (OS_ERR    *)&err    );  
				
				
				if( pMsg[8] == 0x88 && msg_size == 12 )    // back from can1_wheel
				{
				  //can1wheels_SendMessageX( pMsg, pMsg[9], (pMsg[10]<<8)|pMsg[11] );
				}
			
				else     // maybe cmd from host 
				{
				  
				}
*/






/////////////////////////////////////////////////////////////////66 end of file


//printf( " return %d, send Can1wheels data at mailbox: %x. \n", rethal, TxMailbox );
//   printf( "send Can1wheels data. \n" );

//can1wheels_PrepareData();
/*	rethal = can1wheels_GetrxMessageX( rdat, &len, &id );
		if(	rethal == HAL_OK	)
		{
		  can1wheels_SendMessageX( rdat, len, id );
		}
		*/

