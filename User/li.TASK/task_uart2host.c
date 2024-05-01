/**
  ******************************************************************************
  * @file      task_uart2host.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-1-30
  * @brief     RSD79 board app task
	*            handle the demands from host uart
  ******************************************************************************
  **/ 
	

#include "./tasks.h"
#include "../li.BSP/Inc/bsp_usart2host.h"
#include "../li.BSP/Inc/bsp_hostpCtrl.h"


#include "../li.APP/util_misc.h"


//#define  TASK_USART_STK_SIZE      512 
OS_TCB  taskUsartTCB;
CPU_STK  taskUsartStk[ TASK_USART_STK_SIZE ];

////////////////////////////////////37 the task function
void task_Usart( void* p_arg )
{
	(void)p_arg;
	
	OS_ERR   err;
	CPU_SR_ALLOC();   //   cpu_sr =0 for next OS_CRITICAL_ENTER() and OS_CRITICAL_EXIT()
	
  //OS_MSG_SIZE  backmsg_size;
	//uint8_t  *pbackMsg;
	//uint8_t  datafromuart[23];
	
	uint8_t  lenMsg;
	uint8_t  *pMsg;
	
	uint8_t  datas[31];
	uint8_t  dcrc[2];
	uint8_t  dback[3];
	
	
//	
	
  uart2host_Init();  
	printf( "\r\n this is rsd79, are you ready? \r\n " );			
    uart2host_SendString( datas, 4 );
	
	hostp_control_Config();
	p12V_Enable();
	host_Poweron();    // power up host from 24V-12V module 
	
	
	// OSPendMulti() ??
	while( DEF_TRUE ) 
	{  
    ////////13 wait for Q from usart1 or other task 
		pMsg = OSTaskQPend(  (OS_TICK      ) 0,                      // wait Q... forever,
		                     (OS_OPT       ) OS_OPT_PEND_BLOCKING,   // blocking here 
		                     (OS_MSG_SIZE *)&lenMsg,                 // length of message
		                     (CPU_TS      *) 0,                      // need time stamp of the Q ?
									       (OS_ERR      *)&err  );                 // return error 
		                                
	  if( err == OS_ERR_NONE )
	  {
		  if( pMsg != NULL )
		  {
		    for( int i=0; i<lenMsg; i++ )  { datas[i] = pMsg[i];  }   // copy data 
				
				// for test only
				//uart2host_SendString( datas, lenMsg );  datas[11] =0xb1;
				//uart2host_SendString( (uint8_t*)(&datas[11]), 1 );


////////////////////////////29 this is a message from host need be processing				
				if( (uint8_t)(datas[0]) != MSG_BACK_FLAGA )   // this message is maybe from usart1.host
			  {
			    // check cmd length and crc...
					if( datas[0] != lenMsg )    // a wrong message which is bad in length.......
					{ goto  zwrong;        }
					
					util_crc( &(datas[0]), lenMsg-2, dcrc );
					if( dcrc[0] != pMsg[lenMsg-2]  ||  dcrc[1] != pMsg[lenMsg-1] )
					{                        // crc error, should back to host
						//////*** for test .start
						util_crc( &(datas[0]), lenMsg-2, dcrc );
						 uart2host_SendString( datas, lenMsg-2 );
						  uart2host_SendString( dcrc, 2 );
						//////*** for test .end
						
					  // should back for crc error. a wrong message which is bad in crc....... 
						dback[0] = 5; dback[1] = datas[1]; dback[2] = 0xfe;  // back massage: len,cmd,backcode
						util_crc( &(dback[0]), 3, dcrc );
						 uart2host_SendString( dback, 3 );
						  uart2host_SendString( dcrc, 2 );
					}
					
					else    // if it is a valid cmd message, transfer to task_dispatchcmd
					{
					  						
						OSTaskQPost(  (OS_TCB    *)&taskDispatchcmdTCB, 
		                      (void      *) datas, 
		                      (OS_MSG_SIZE) lenMsg-2,        // let off crc bytes 
		                      (OS_OPT     ) OS_OPT_POST_FIFO,
		                      (OS_ERR    *)&err    );  
													
								
				/*		// I have received your cmd, will process it.
						dback[0] = 5; dback[1] = datas[1]; dback[2] = 0x00;  // back massage: len,cmd,backcode
						util_crc( &(dback[0]), 3, dcrc );
					   uart2host_SendString( &(datas[1]), lenMsg-3 );
						  uart2host_SendString( dcrc, 2 );        */			
													
					}
			  }
			

////////////////////////////29 this is a message from other task back to host.......
			  else    /* this is a back message should send to host  */
			  //{
			    if( (datas[lenMsg-2] == MSG_BACK_FLAGY) && (datas[lenMsg-1] == MSG_BACK_FLAGZ)  )
					{
					  // calculate crc first...
					
						//util_crc( &(datas[1]), lenMsg-3, &(datas[lenMsg-2]) );    // maybe pMsg could not be writen which in OS
					  util_crc( &(datas[1]), lenMsg-3, dcrc );
					
						//OS_CRITICAL_ENTER();
					   uart2host_SendString( &(datas[1]), lenMsg-3 );
						  uart2host_SendString( dcrc, 2 );
						//OS_CRITICAL_EXIT();
					}
			  //}
		  }
	  }
		
		
		//dcrc[0] = 0xa7;  dcrc[1] = 0x8b; 
		//uart2host_SendString( dcrc, 2 );
		//OSTimeDlyHMSM( 0, 0, 2, 0, OS_OPT_TIME_DLY, &err );
		
//////////////////////////////////////////////////////////////////////71		
    zwrong:  {  ; }
	}
	
}










/////////////////////////////////////////////////////////////////66 end of file





/*	  OS_CRITICAL_ENTER();            // should not crack in send data duration
		
		OSTaskQPost(  (OS_TCB    *)&taskDispatchcmdTCB, 
		              (void      *) usart2host_Rxbuf, 
		              (OS_MSG_SIZE) usart2host_Rxbuf[0], 
		              (OS_OPT     ) OS_OPT_POST_FIFO,
		              (OS_ERR    *)&err    );  
		
		
		OS_CRITICAL_EXIT();             // back from critical region
*/		
	
		
		//OSTimeDlyHMSM( 0, 0, 0, 1, OS_OPT_TIME_DLY, & err );
    //memset( usart2host_Rxbuf, 0, USART2HOST_RXBUFF_SIZE );     // clear received data 						
		
									
		// wait back message from tasks...						
		//printf( "xy\n" );
		//HAL_UART_Transmit( &uart_Handle, usart2host_Rxbuf, 30, 1000 );	// how to get length of frame 
		
   /* pbackMsg = OSTaskQPend(  (OS_TICK      ) 0,                      // wait sem... forever,
		                         (OS_OPT       ) OS_OPT_PEND_BLOCKING,   // blocking here 
		                         (OS_MSG_SIZE *)&backmsg_size,
		                         (CPU_TS      *) 0,                      // I need time stamp of the sem
									           (OS_ERR      *)&err   );                 // return error 
		
		if( err == OS_ERR_NONE )
		{  printf( "backmsg:%s\n", pbackMsg );   }   // ??
		*/

/*    // task sem from HAL_UART_AbortReceiveCpltCallback(), in theisrs.c
		OSTaskSemPend(  (OS_TICK   ) 0,                      // wait sem... forever,
									  (OS_OPT    ) OS_OPT_PEND_BLOCKING,   // blocking here
									  (CPU_TS   *) 0,                      // I need time stamp of the sem
									  (OS_ERR   *)&err  );                 // return error
*/
		
    
//    printf( "received data: %s\n", usart2host_Rxbuf );
