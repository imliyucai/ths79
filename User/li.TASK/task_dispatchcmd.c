/**
  ******************************************************************************
  * @file      task_dispatchcmd.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-2-6
  * @brief     RSD79 board app task
	*            dispatch commands from host to demanded tasks
  ******************************************************************************
  **/ 
	



#include "./tasks.h"
#include "./cmds.h"

#include "../li.uCOS/uCOS_bsp.h"
//#include "../BSP/Inc/bsp_usart2host.h"


char thisfirmware_version[14] = "A3021120240319";  

//#define  TASK_DISPATCHCMD_STK_SIZE   256
OS_TCB  taskDispatchcmdTCB;
CPU_STK  taskDispatchcmdStk[ TASK_DISPATCHCMD_STK_SIZE ];

uint8_t  hostcmd[108];
uint8_t  backmsg[29] = { 0x99,   0x0B,0x20,0x43,0x06,0x01,0x00,0x00,0x02,0x01,   0xe9,0xf9 }; 


////////////////////////////////////37 the task function
void task_Dispatchcmd( void *p_arg )
{
	(void)p_arg;
	
	OS_ERR   err;
  CPU_SR_ALLOC();    // cpu_sr, for OS_CRITICAL_ENTER() 
	
		
	OS_MSG_SIZE  msg_size;
	OS_FLAGS  flags_rdy;
	
	uint8_t   *pMsg;
	
	
  
//	printf( "\r\n I will dispatch command to tasks. \r\n " );				 
	
	
	while( DEF_TRUE ) 
	{  
    ////////13 wait for host command from task_uart2host
    ///*** should pend Q forever ??
		pMsg = OSTaskQPend(  (OS_TICK      ) 120,                      // wait Q... forever, ??
		                     (OS_OPT       ) OS_OPT_PEND_BLOCKING,   // blocking here 
		                     (OS_MSG_SIZE *)&msg_size,
		                     (CPU_TS      *) 0,                      // need time stamp of the Q ?
									       (OS_ERR      *)&err  );                 // return error 
		                                  
		
		if( err == OS_ERR_NONE )
		{
		  //OS_CRITICAL_ENTER();            // should not crack in send data duration
			
			for( int i=0; i<msg_size; i++ )  {  hostcmd[i] = pMsg[i];   }   // copy data
			
			// frame length and crc has checked in task_uart2host
			// analysis cmd frame and dispatch to goal tasks...
			switch(  hostcmd[1] )   // hostcmd[0] is frame length
			{
			  case HOSTCMD_SETWHEELSVELOCITY10:   // should push to task_can1wheels
				case HOSTCMD_SETWHEELSVELOCITY:   	
				  OSTaskQPost(  (OS_TCB    *)&taskCan1wheelsTCB, 
		                    (void      *) &(hostcmd[1]), 
		                    (OS_MSG_SIZE) (msg_size-1), 
		                    (OS_OPT     ) OS_OPT_POST_FIFO,
		                    (OS_ERR    *)&err    );
				
				
					break; 
				
				
					//break; 
				
				
												
												
					
												
												
												
												
												
												
//////////////////////////////31 the polling cmd should be processing in own task.......						
				case HOSTCMD_GETMAINCLEANSTATUS: 
          // get status from FLAG_GRP
          flags_rdy = OSFlagPend(  (OS_FLAG_GRP *)&AppSysStatus,
                                   (OS_FLAGS     )( 0xffffffff ),
                                   (OS_TICK      )0,         // when any flags are 1 to...
                                   (OS_OPT       ) OS_OPT_PEND_NON_BLOCKING +OS_OPT_PEND_FLAG_SET_ANY,
                                   (CPU_TS      *)0,
                                   (OS_ERR      *)&err  );  

				//flags_rdy = AppSysStatus.Flags;
				  backmsg[1] =8;  backmsg[2] =HOSTCMD_GETMAINCLEANSTATUS;
				  backmsg[3]=flags_rdy&0x000000ff;        backmsg[4]=(flags_rdy&0x0000ff00)>>8; 
				  backmsg[5]=(flags_rdy&0x00ff0000)>>16;  backmsg[6]=(flags_rdy&0xff000000)>>24; 
				  backmsg[7]=0xe9; backmsg[8]=0xf9;
				   // next send to task_usart2host
				  OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		                    (void      *)backmsg, 
		                    (OS_MSG_SIZE) (8+1), 
		                    (OS_OPT     ) OS_OPT_POST_FIFO,
		                    (OS_ERR    *)&err    );						
         break;








//////////////////////////////31 some misc cmd should back to host in myself												
					case HOSTCMD_GETFIRMWAREVER:	
					  backmsg[1] = 18;   // length of all
            backmsg[2] = HOSTCMD_GETFIRMWAREVER;    // command byte
					  //  int i=0;
					  for( int i=0; i<14; i++ ) backmsg[3+i] = thisfirmware_version[i];
					  backmsg[17] = MSG_BACK_FLAGY; 
					  backmsg[18] = MSG_BACK_FLAGZ; 
										
					  OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		                      (void      *)backmsg, 
		                      (OS_MSG_SIZE) (18+1), 
		                      (OS_OPT     ) OS_OPT_POST_FIFO,
		                      (OS_ERR    *)&err    );						
            break;	

											
					default:  break;
			
			}
			
			
			
						
			
			
			
			
			
			
			
			//OS_CRITICAL_EXIT();             // back from critical region
		
		}
		
		

//////////////////////////////////////////////////////////////////////71	test only	
	/*	OSTaskQPost(  (OS_TCB    *)&taskUsartTCB, 
		              (void      *) backmsg, 
		              (OS_MSG_SIZE) 12, 
		              (OS_OPT     ) OS_OPT_POST_FIFO,
		              (OS_ERR    *)&err    );    */
		
	
		
	}
	
}









/////////////////////////////////////////////////////////////////66 end of file

/*    // task sem from HAL_UART_AbortReceiveCpltCallback(), in usart.c
		OSTaskSemPend(  (OS_TICK   ) 0,                      // wait sem... forever,
									  (OS_OPT    ) OS_OPT_PEND_BLOCKING,   // blocking here
									  (CPU_TS   *) 0,                      // i need time stamp of the sem
									  (OS_ERR   *)&err  );                 // return error   */


