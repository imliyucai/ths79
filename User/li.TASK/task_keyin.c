/**
  ******************************************************************************
  * @file      task_keyin.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-1-30
  * @brief     RSD79 board app task
	*            handle keyin events
  ******************************************************************************
  **/ 
	

#include "./tasks.h"
#include "../li.BSP/Inc/bsp_key.h"


#include "../li.uCOS/uCOS_bsp.h"



//#define  TASK_KEY_STK_SIZE        128 
OS_TCB  taskKeyTCB;
CPU_STK  taskKeyStk[ TASK_KEY_STK_SIZE ];


////////////////////////////////////37 the task function
void task_Key( void* p_arg )
{
	(void)p_arg;
	
	OS_ERR     err;
	CPU_SR_ALLOC();   // cpu_sr, for OS_CRITICAL_ENTER
	
	
	CPU_TS_TMR    ts_int;
	CPU_INT32U     cpu_clk_freq; 
	cpu_clk_freq = BSP_CPU_ClkFreq();       // get CPU clock, for time stamp
	
	
	

	keyint_Config();
	din_Config();
  mint_Config();
	

	uint8_t din8,ddin;
	uint8_t din4min4;
  
  uint8_t testsend[17] = {0x99, 0xff, 0xe9,0xf9};
	uint32_t  inall=0x0;
	uint32_t  testdd;
	
/*	while( DEF_TRUE ) 
	{                                  //
		
									
		for(uint8_t i=0; i<8; i++)
    {
			dinx = din_Scan(i);									
      printf( "key %d = %d\n", i, dinx );
		}	
		OSTimeDlyHMSM( 0, 0, 3, 0, OS_OPT_TIME_DLY, & err );
		
    	// wait sem from keyint isr 
		OSTaskSemPend(  (OS_TICK   ) 0,                      // wait keyin... forever
									  (OS_OPT    ) OS_OPT_PEND_BLOCKING,   // blocking here
									  (CPU_TS   *) 0,                      // I need time stamp of keyin
									  (OS_ERR   *)&err  );                 // return error
		
		ts_int = CPU_IntDisMeasMaxGet();       // uc/os function, get time duration in isr

		//OS_CRITICAL_ENTER();            // now should not crack when send data

		printf( "keyin interrupt %d us.\r\n", ts_int / ( cpu_clk_freq / 1000000 ) );        		
		
		//OS_CRITICAL_EXIT();             // back from critical region
		
	
	}  */
	
//////////////////////////////31 check keyin status to store system state in OS_FLAG_GRP	
	while( DEF_TRUE ) 
	{                                  //
	
		
		//dinx = din_Scan(i);
		din8 =din_ScanAll( &ddin );  
		din4min4 =mint_Scan();
		
		if( key9_Check() == KEYCLOSED )   // 
		{ din4min4 |= 0x10; }
		else
		{ din4min4 &= ~0x10; }
		
		inall =0;
		inall |= (uint32_t)din8;                       
	  inall |= ((uint32_t)din4min4)<<8; 
		
////// all key status post to 	OS_FLAG_GRP, donnot process it in this task.......		
		//////////////////////////31 setting to FLAG_GRP 
		/*  OSFlagPost(  (OS_FLAG_GRP *)&AppSysStatus,
                 (OS_FLAGS     )inall,    // testd
                 (OS_OPT       )OS_OPT_POST_FLAG_SET,    // OS_OPT_POST_FLAG_SET _clr=set this bit(one flag)
                 (OS_ERR      *)&err  );     */
		
		//testdd = AppSysStatus.Flags;
		AppSysStatus.Flags &= 0xffffe000;
		AppSysStatus.Flags |= inall;     // how to ???  b.bbbb.bbbbbbbb
		
		
		
		
		
		
		
		//*** for test only .start
		testsend[1] = din8;   testsend[2] =0xe9;  testsend[3] =0xf9;    
		OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		              (void      *)&testsend, 
		              (OS_MSG_SIZE) 4, 
		              (OS_OPT     ) OS_OPT_POST_FIFO,
		              (OS_ERR    *)&err    );  
									
		testsend[1] = inall&0x000000ff;			testsend[2] = (inall&0x0000ff00)>>8;		
    testsend[3] = (inall&0x00ff0000)>>16;					testsend[4] = (inall&0xff000000)>>24;	
		testsend[5] =0xe9;  testsend[6] =0xf9;    							
		OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		              (void      *)&testsend, 
		              (OS_MSG_SIZE) 7, 
		              (OS_OPT     ) OS_OPT_POST_FIFO,
		              (OS_ERR    *)&err    );  							
		//*** for test only .end   

									
	 	OSTimeDlyHMSM( 0, 0, 0, 607, OS_OPT_TIME_DLY, & err );		// for turn over OS task must						
	}
	
}










/////////////////////////////////////////////////////////////////66 end of file






