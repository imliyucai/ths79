/**
  ******************************************************************************
  * @file      util_time.h
  * @author    liyc
  * @version   V1.0
  * @date      2024-4-21
  * @brief     header of utility functions for wait or delay time
	*             
  ******************************************************************************
  **/ 

#ifndef __UTIL_TIME_H
#define __UTIL_TIME_H 	

#include <stdint.h>	  
//////////////////////////////////////////////////////////////////////////////////  

////////////////////////////////////////////////////////////////////////////////// 	 
void wait_Init( uint8_t SYSCLK );

void util_waitms_xostick( uint16_t nms );
void util_waitus_xostick( uint32_t nus );


//////////////////////////////////////////43 time wait from NOP
void util_wait98ns( uint8_t n98ns );
void util_wait2us( uint8_t n2us );
void util_wait97us( uint8_t n97us );
void util_wait3ms( uint8_t n3ms );
void util_wait23ms( uint8_t n23ms );

void util_waitms( uint32_t nms );
void util_wait(uint32_t n);



#endif   // __UTIL_TIME_H

 


/////////////////////////////////////////////////////////////////66 end of file


























