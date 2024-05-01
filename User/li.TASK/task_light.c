/**
  ******************************************************************************
  * @file      task_light.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-1-30
  * @brief     RSD79 board app task
	*            handle the demand to light on/off the leds 
  ******************************************************************************
  **/ 
	


#include "./tasks.h"
#include "../li.BSP/Inc/bsp_led.h"
#include "../li.BSP/Inc/bsp_color2812Ctrl.h"




//#define  TASK_LIGHT_STK_SIZE     64
OS_TCB  taskLightTCB;
CPU_STK  taskLightStk[ TASK_LIGHT_STK_SIZE ];


////////////////////////////////////37 the task function
void task_Light( void *p_arg )
{
	(void)p_arg;
	
	OS_ERR err;
	CPU_SR_ALLOC();   // for OS_CRITICAL_ENTER
	
  uint8_t rgb[3]={0x33,0x33,0x33};  // red-green-blue or green-red-blue ?
	uint8_t rgbK[24]={ 0xA0,0x0C,0x05, 0x07,0x9C,0x06, 0x03,0x0C,0xA5, 
	                  0xA1,0x9C,0x05, 0xB0,0x0C,0xA5, 0x04,0x9C,0xA5, 
	                 0x87,0x88,0x89, 0x50,0x9C,0x05 };  
	
	uint16_t  n=0;								 
	leds_Config(); 
					 
	color2812_ctrl_Config();
	
//	color2812_testLight( grb );
	color2812_RunXBar2( rgbK, 6 );
	
	while( DEF_TRUE ) 
	{                                             //
		
		//LED1_TOGGLE;  //LED2_OFF;
		//OSTimeDly( 5000, OS_OPT_TIME_DLY, &err );    // delay in ticks
		OSTimeDlyHMSM( 0, 0, 1, 0, OS_OPT_TIME_DLY, & err );        // delay in real times
		
	//	color2812_RunLDx( rgb );
		
		
		//LED1_OFF;  //LED2_ON;
		LED2_TOGGLE;
		OSTimeDlyHMSM( 0, 0, 3, 0, OS_OPT_TIME_DLY, & err );        //
		
		
		OS_CRITICAL_ENTER();
		
		color2812_RunLDx( rgbK+3*n ); 
		color2812_RunXBar2( rgbK+3*n, 7 );
		n++; if(n>7) n=0;
		
		OS_CRITICAL_EXIT();             // back from critical region
	}
}












/////////////////////////////////////////////////////////////////66 end of file


