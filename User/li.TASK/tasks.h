/**
  ******************************************************************************
  * @file      tasks.h
  * @author    liyc
  * @version   V1.0
  * @date      2024-1-30
  * @brief     RSD79 board app
	*            declaration of all task functions and variables
  ******************************************************************************
  **/ 
	
  
#ifndef __TASKS_H
#define	__TASKS_H	
	
	

/*
****************************************************************************************
*                                         STANDARD LIBRARIES
****************************************************************************************
*/


#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <math.h>


/*
****************************************************************************************
*                                                 OS
****************************************************************************************
*/

#include  "os.h"


/*
****************************************************************************************
*                                              LIBRARIES
****************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>


/*
****************************************************************************************
*                                              APP / BSP
****************************************************************************************
*/



// some flags for between tasks
#define  MSG_BACK_FLAGA     (0x99)
#define  MSG_BACK_FLAGY     (0xe9)
#define  MSG_BACK_FLAGZ     (0xf9)

#define  CAN1_BACK_FLAG    (0x88)

//////////////////////////////31 tasks priority
#define  TASK_USART_PRIO    3
#define  TASK_LIGHT_PRIO     9
#define  TASK_KEY_PRIO      5
#define  TASK_DISPATCHCMD_PRIO  4

#define  TASK_CAN1WHEELS_PRIO      6
#define  TASK_DOWORK_PRIO      6
#define  TASK_WISTATION_PRIO      7

//////////////////////////////31 tasks stack
#define  TASK_USART_STK_SIZE      512 
#define  TASK_LIGHT_STK_SIZE     64
#define  TASK_KEY_STK_SIZE        128 
#define  TASK_DISPATCHCMD_STK_SIZE   256

#define  TASK_CAN1WHEELS_STK_SIZE      128 
#define  TASK_DOWORK_STK_SIZE      128 
#define  TASK_WISTATION_STK_SIZE      128 

extern CPU_STK  taskLightStk[ ];
extern CPU_STK  taskKeyStk[ ];

extern CPU_STK  taskUsartStk[ ];
extern CPU_STK  taskDispatchcmdStk[ ];

extern CPU_STK  taskCan1wheelsStk[ ];
extern CPU_STK  taskDoworkStk[ ];
extern CPU_STK  taskWiStationStk[ ];


//////////////////////////////31 tasks control block
extern OS_TCB  taskLightTCB;
extern OS_TCB  taskKeyTCB;

extern OS_TCB  taskUsartTCB;
extern OS_TCB  taskDispatchcmdTCB;

extern OS_TCB  taskCan1wheelsTCB;
extern OS_TCB  taskDoworkTCB;
extern OS_TCB  taskWiStationTCB;




////////////////////////////29 other os resource
extern  OS_FLAG_GRP  AppSysStatus;


/////////////////////////////////////////////////////////////////
////////////////////////////////////37 tasks function decleration 
void task_Light( void* p_arg );
void task_Key( void* p_arg );

void task_Usart( void* p_arg );
void task_Dispatchcmd( void* p_arg );

void task_Can1wheels( void* p_arg );
void task_Dowork( void* p_arg );
void task_wiStation( void* p_arg );






#endif   /* __TASKS_H */


/////////////////////////////////////////////////////////////////66 end of file

