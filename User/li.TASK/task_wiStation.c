/**
  ******************************************************************************
  * @file      task_wiStation.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-3-29
  * @brief     RSD79 board app task
	*            do the charge or other work with station
  ******************************************************************************
  **/ 
	




#include "./tasks.h"
#include "../li.BSP/Inc/bsp_cmt2300a.h"
#include "../li.DRIVER/drv_cmt2300a.h"

#include "../li.APP/util_time.h"





//#define  TASK_WISTATION_STK_SIZE      128 
OS_TCB  taskWiStationTCB;
CPU_STK  taskWiStationStk[ TASK_WISTATION_STK_SIZE ];


////////////////////////////////////37 the task function
void task_wiStation( void* p_arg )
{
	(void)p_arg;
	
	OS_ERR   err;
	CPU_SR_ALLOC();   // for OS_CRITICAL_ENTER

	
/*	          */
	uint8_t dat[20] = {0x12,0x23,0x34,0x45,0x56,0x67,  0x78,0x89,0x9a,0xab, 
	                    0xbc,0xcd,0xde,  0xef,0xf0,0x01,0x13,0x46,0x79, 0xac };  
	
	uint8_t rdat[21] = {0x11,0x22,0x33,0x44,0x55,0x66,  0x77,0x88,0x99,0xAA, 
	                    0xbb,0xcc,0xdd,  0xee,0xff,0xa1,0xb2,0xc3,0xd4, 0xe5 };
											
	CMT2300A_IO_Init();
	RF433M_Init();
	
	//util_waitms(10);
	util_wait97us( 47 );
											
    //CMT2300A_EnableReadFifo(); 
    //CMT2300A_GoRx();
	
	while( DEF_TRUE ) 
	{  

		OS_CRITICAL_ENTER();            // should not crack in send data duration
    
 
		
		if( CMT2300A_RecieveData( rdat, 20 ) ==TRUE )
		{ CMT2300A_SendData( rdat, 20 ); }	
		//rdat[1] = cmt23_checkstatus61();
		//rdat[5] = cmt23_checkstatus65();
		//rdat[6] = cmt23_checkstatus66();
		//rdat[7] = cmt23_checkstatus67();
		//rdat[8] = cmt23_checkstatus68();
		//rdat[9] = cmt23_checkstatus6d();
		//rdat[10] = cmt23_checkstatus6e();
		//rdat[11] = cmt23_checkstatus70();
		
		CMT2300A_SendData( dat, 20 );  dat[19]++;
	//	rdat[0] = cmt23_checkstatus61();
		
		
		OS_CRITICAL_EXIT();             // back from critical region
		
	  OSTimeDlyHMSM( 0, 0, 1, 0, OS_OPT_TIME_DLY, & err );  
		
		//CMT2300A_SendData( dat, 11 );
		OSTimeDlyHMSM( 0, 0, 2, 0, OS_OPT_TIME_DLY, & err );  
		//CMT2300A_SendData( rdat, 20 );
		OSTimeDlyHMSM( 0, 0, 1, 0, OS_OPT_TIME_DLY, & err );  
		
	}
	
}










/////////////////////////////////////////////////////////////////66 end of file


