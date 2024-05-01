/**
  ******************************************************************************
  * @file      util_misc.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-4-21
  * @brief     utility functions for system misc
	*             
  ******************************************************************************
  **/ 


#include "stm32f4xx.h"
#include "./util_misc.h"








// {0x0B,0x20,0x43,0x06,0x01,0x00,0x00,0x02,0x01};  // to 0x41d2
void util_crc( uint8_t *dats, uint16_t dlen, uint8_t *dcrc )
{

  uint16_t poly = 0x8408;
  uint16_t crc;
  int j, ib, carry;

  crc = 0;
  for( j=0; j<dlen; j++ )
  {
    crc = crc ^ (uint16_t)(dats[j]);
    for( ib=0; ib<8; ib++ )
    {
      carry = crc & 1;
      crc = crc / 2;
      if( carry )
      {
        crc = crc ^ poly;
      }
    }
  }
  dcrc[0] = (uint8_t)((crc&0xff00)>>8);
  dcrc[1] = (uint8_t)((crc&0x00ff));
}


////////////////////////////////////////////////////////57



/////////////////////////////////////////////////////////////////66 end of file


/*  int Poly = 0x8408;
    
  unsigned int Crc;
  int j, i_bits, carry;
  
    int lg = 9;
    unsigned int Frame[] = {0x0B,0x20,0x43,0x06,0x01,0x00,0x00,0x02,0x01};  // to 0x41d2

  Crc = 0;
  for( j=0; j<lg; j++ )
  {
    Crc = Crc ^ Frame[j];
    for( i_bits=0; i_bits<8; i_bits++ )
    {
      carry = Crc & 1;
      Crc = Crc / 2;
      if( carry )
      {
        Crc = Crc ^ Poly;
      }
    }
  }
    printf( “CRC = %x ”, Crc);
*/

