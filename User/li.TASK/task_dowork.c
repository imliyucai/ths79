/**
  ******************************************************************************
  * @file      task_dowork.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-2-7
  * @brief     RSD79 board app task
	*            do the clean work main task
  ******************************************************************************
  **/ 
	

#include "./tasks.h"
#include "../li.BSP/Inc/bsp_workmCtrl.h"
#include "../li.BSP/Inc/bsp_worknqCtrl.h"
#include "../li.BSP/Inc/bsp_dcmCtrl.h"

#include "../li.BSP/Inc/bsp_rs485Signals.h"






//#define  TASK_DOWORK_STK_SIZE      128 
OS_TCB  taskDoworkTCB;
CPU_STK  taskDoworkStk[ TASK_DOWORK_STK_SIZE ];


////////////////////////////////////37 the task function
void task_Dowork( void *p_arg )
{
	(void)p_arg;
	
	OS_ERR   err;
	CPU_SR_ALLOC();   // for OS_CRITICAL_ENTER
	
	workm_signals_Config();
	
  workm1_toRun();
	workm2_toRun();

  worknq_signals_Config();
	worknq2_Powerup();
	worknq3_Powerup();
	
	dcms_signals_Config();
	dcm_toRun( 0xd0, Bit_RESET );    // GPIO_PIN_SET
	dcm_toRun( 0xd1, Bit_SET );
	dcm_toRun( 0xd2, Bit_RESET );
	dcm_toRun( 0xd3, Bit_SET );
	dcm_toRun( 0xd4, Bit_RESET );  
	 
	
	
	rs485_Init();
	uint8_t dat[7] = {0x31,0x32,0x33,0x44,0x45,0x46,0x49};
	
	while( DEF_TRUE ) 
	{  

		OS_CRITICAL_ENTER();            // should not crack in send data duration
    
 
		
		rs485_SendFrame( dat, 7 );
		dat[6]++;
		//if( dat[6] ==  )

		
		
		
		OS_CRITICAL_EXIT();             // back from critical region
		
	  OSTimeDlyHMSM( 0, 0, 4, 0, OS_OPT_TIME_DLY, & err );  
		
	}
	
}










/////////////////////////////////////////////////////////////////66 end of file


