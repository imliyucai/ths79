/**
  ******************************************************************************
  * @file      cmds.h
  * @author    liyc
  * @version   V1.0
  * @date      2024-3-19
  * @brief     RSD79 board app
  *            commands from host
  ******************************************************************************
  **/ 
	
  
#ifndef __CMDS_H
#define	__CMDS_H	
	
	

/*
****************************************************************************************
*                                         STANDARD LIBRARIES
****************************************************************************************
*/

/*
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <math.h>
	*/

/*
****************************************************************************************
*                                                 MACRO ALL
****************************************************************************************
*/

#define  THISFIRMWARE_VER   (A3.02.11.20240319) 
//char thisfirmware_version[14] = "A3021120240319";  

/*
****************************************************************************************
*                                              LIBRARIES
****************************************************************************************
*/


/*
****************************************************************************************
*                                              APP / BSP
****************************************************************************************
*/




#define  HOSTCMD_SETWHEELSVELOCITY     (0x15)
#define  HOSTCMD_SETWHEELSVELOCITY10     (0x28)


#define  HOSTCMD_GETFIRMWAREVER      (0x48)
#define  HOSTCMD_GETMAINCLEANSTATUS     (0x46)

/////////////////////////
#define HOSTCMD_UPGRADEFIRMWARE_A     (0x71)
#define HOSTCMD_UPGRADEFIRMWARE_D    (0x72)
#define HOSTCMD_UPGRADEFIRMWARE_Z     (0x73)




#endif   /* __CMDS_H */


/////////////////////////////////////////////////////////////////66 end of file

