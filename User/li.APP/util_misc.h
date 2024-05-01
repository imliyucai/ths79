/**
  ******************************************************************************
  * @file      util_misc.h
  * @author    liyc
  * @version   V1.0
  * @date      2024-3-28
  * @brief     the misc declerations of functions and veriables
  *            
  ******************************************************************************
  **/ 


#ifndef __UTIL_MISC_H
#define	__UTIL_MISC_H



#include <stdint.h>


typedef unsigned char  BOOL;
typedef unsigned char  bool8_t;

#ifndef FALSE
#define FALSE     0
#endif

#ifndef TRUE
#define TRUE      (!FALSE)
#endif




void util_crc( uint8_t *dats, uint16_t dlen, uint8_t *dcrc );







#endif /* __UTIL_MISC_H */


/////////////////////////////////////////////////////////////////66 end of file




