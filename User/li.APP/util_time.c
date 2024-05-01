/**
  ******************************************************************************
  * @file      util_time.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-4-21
  * @brief     utility functions for wait or delay time
	*             
  ******************************************************************************
  **/ 

#include "./util_time.h"
 
#include "stm32f4xx.h" 

////////////////////////////////////////////////////////////////////////////////// 	 

//#define  SYSTEM_SUPPORT_UCOS   (1)
// if use uC/OS, include this header
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					// uC/OS header file  
#endif 
//////////////////////////////////////////////////////////////////////////////////  

////////////////////////////////////////////////////////////////////////////////// 
 
static uint8_t  fac_us=0;  // us multip			   
static uint16_t  fac_ms=0;  // ms multip, when use uC/OS, equal ms of one tick

			   

// when uC/OS, will Init tick of uC/OS ?
// SYSTICK = HCLK/8
// SYSCLK as input parameter
void wait_Init( uint8_t sysclkFreq )
{
  // HCLK=168MHz /8  as systick source 
// 	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK_Div8 );
	
	fac_us = sysclkFreq /8;		       // sysclkFreq=HCLK, /8 is same as systick
	    
	fac_ms = (uint16_t)fac_us*1000;     //    

}								    

//#define  OS_CRITICAL_METHOD
#ifdef OS_CRITICAL_METHOD 	// OS_CRITICAL_METHOD use uC/OS

   								   
void util_waitus( uint32_t nus )
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload = SysTick->LOAD;	//
	
  ticks = nus*fac_us; 			 // 
	tcnt = 0;
 OSSchedLock();				// donnot interrupt when wait
	
	told = SysTick->VAL;        	
	while(1)
	{
		tnow = SysTick->VAL;	
		if( tnow != told )
		{	    
			if( tnow<told )   tcnt += told-tnow;  // down counter in tick timer
			else tcnt += reload-tnow+told;	    
			told = tnow;
			if( tcnt >= ticks )  break;    // wait time done
		}  
	};
 OSSchedUnlock();			// back could interrupt 									    
}

void util_waitms( uint16_t nms )
{	
	if( OSRunning==OS_TRUE && OSLockNesting==0 )   // uC/OC is running	   
	{		  
		if( nms >= fac_ms )    // 
		{
   		OSTimeDly( nms/fac_ms );	  // uC/OS delay out
		}
		nms %= fac_ms;				// the little time
	}
	util_waitus( (uint32_t)( nms*1000) );		//
}


#else  // if not uC/OS used
////////////////////////////////////
void util_waitus_xostick( uint32_t nus )
{		
	uint32_t temp;	    	 
	SysTick->LOAD = nus*fac_us; 
	SysTick->VAL = 0x00;        
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;          
	do
	{
	  temp = SysTick->CTRL;
	}
	while( (temp&0x01) && !(temp&(1 << 16)) );
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       
	SysTick->VAL = 0x00;       // clear counter	 
}

// when 168MHz, nms<=798ms 
static void wait_xms( uint16_t nms )
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD = (uint32_t)nms*fac_ms;   // 
	SysTick->VAL = 0x00;           // clear counter
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;          // start down counter  
	do
	{
	  temp = SysTick->CTRL;
	}
	while( (temp&0x01) && !(temp&(1 << 16)) );  // wait...   
	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       // close counter
	SysTick->VAL = 0x00;       // clear counter	  	    
} 

// nms:0~65535
void util_waitms_xostick( uint16_t nms )
{	 	 
	uint8_t repeat = nms/540;	
	uint16_t remain = nms%540;

	while( repeat )
	{
		wait_xms( 540 );
		repeat--;
	}
	if( remain )
	  wait_xms( remain );
	
} 

#endif
			 


//////////////////////////////////////////43 time wait from NOP
void util_wait98ns( uint8_t n98ns )
{
  uint8_t i;
	for( i=0; i<n98ns; i++ )   // run in 168MHz core frequency
	{ 
		__nop(); __nop(); __nop(); __nop(); __nop();     
	}
}

void util_wait2us( uint8_t n2us )
{
  uint8_t i,j;
	for( i=0; i<n2us; i++ )
	{ 
		//__nop(); __nop(); __nop(); __nop(); __nop(); 
		for( j=0; j<20; j++) 
    { util_wait98ns(1); }		
	}
}

void util_wait97us( uint8_t n97us )
{
  uint32_t i,j;
  for( i=0; i<n97us; i++ )
  {     // 1297==97us; 12987 == 1ms @168MHz
    for( j=0; j<1297; j++ ) 
    {;}
  }
}

void util_wait3ms( uint8_t n3ms )
{
  uint32_t j;
	uint8_t i;
  for( i=0; i<n3ms; i++ )
  {     // 1297==97us; 12987 == 1ms @168MHz
    for( j=0; j<38961; j++ ) 
    {;}
  }
}

void util_wait23ms( uint8_t n23ms )
{
  uint32_t j;
	uint8_t i;
  for( i=0; i<n23ms; i++ )
  {     // 1297==97us; 12987 == 1ms @168MHz
    for( j=0; j<298700; j++ ) 
    {;}
  }
}

void util_waitms( uint32_t nms )
{
  uint32_t i,j;
  for( i=0; i<nms; i++ )
  {     // 12987 == 1ms @168MHz
    for( j=0; j<12987; j++ ) 
    {;}
  }
}


void util_wait( uint32_t n )
{
  uint32_t i;
  for( i=0; i<n; i++ )
  { ;;     }
}

/////////////////////////////////////////////////////////////////66 end of file


































