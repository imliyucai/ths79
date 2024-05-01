/**
  ******************************************************************************
  * @file      drv_cmt2300a.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-4-10
  * @brief     RSD79 board bsp
	*            driver source file of rf_chip cmt2300a 
  ******************************************************************************
  **/ 


#include  "../../User.stdlib/li.BSP/Inc/bsp_cmt2300a.h"
#include "./drv_cmt2300a.h"

#include "stm32f4xx.h"

#include "../../User/li.APP/util_misc.h"
#include "../../User/li.APP/util_time.h"



//////////////////////////////////////////////47	
static void CMT2300A_SendByte( uint8_t data8 )
{
  uint8_t i;
	for( i=0; i<8; i++ )
	{
	  CMT_SCLK_OUT_L();
		if( data8 & 0x80 )  CMT_SDIO_OUT_H();  /* I will read data from you, OK? */
		else CMT_SDIO_OUT_L();                 /* I will write data to you, please?  */
		util_wait98ns(4);  		   // > 0.5 sclk clock (0.5us)   11-7-6..5
		data8 <<= 1;           // next bit ready
		CMT_SCLK_OUT_H();    // you can read this bit now
		util_wait98ns(4);		     // > 0.5 sclk clock (0.5us)    11-7-6..5
	}
}
static uint8_t CMT2300A_RecvByte( void )
{
  uint8_t i;
  uint8_t data8=0xff;

  for( i=0; i<8; i++ )
  {
	  CMT_SCLK_OUT_L();
		util_wait98ns(4);     // > 0.5 sclk clock (0.5us)  wait98ns(5)=400ns
		data8 <<= 1;
		CMT_SCLK_OUT_H();
		if( CMT_SDIO_IN() )  data8 |= 0x01;
		else  data8 &= ~0x01;
		util_wait98ns(4);     // > 0.5 sclk clock (0.5us)   
	}
  return data8;	
}    // access cmt2300a in bit frequency 1.38MHz, bit time 720ns 

//////////////////////////////////////////////////////////59
void CMT2300A_WriteReg( uint8_t addr, uint8_t dat )
{
  CMT_SDIO_OUT_H();  // ? 
	CMT_SCLK_OUT_L();  // ?
	CMT_FCSB_OUT_H();  // ?
	
	CMT_CSB_OUT_L();   util_wait98ns(11); 
	//util_waitus(1);   /* > 0.5 SCLK cycle */
	
	// turn over CMT_SDIO to output mode
	CMT2300A_Turnto_sdioOUT();
	  CMT2300A_SendByte( addr & 0x7f );  /* MSB is 0 for write data flag to CMT */
	  CMT2300A_SendByte( dat );
	
	CMT_SCLK_OUT_L();   util_wait98ns(11); 
	//util_waitus(1);   /* > 0.5 SCLK cycle */
	CMT_CSB_OUT_H();
	
	CMT_SDIO_OUT_H();  CMT_SDIO_IN();
	CMT_FCSB_OUT_H();
}
void CMT2300A_ReadReg( uint8_t addr, uint8_t *pdat )
{
  CMT_SDIO_OUT_H();  // ? 
	CMT_SCLK_OUT_L();  // ?
	CMT_FCSB_OUT_H();  // ?
	
	CMT_CSB_OUT_L();   util_wait98ns(11); 
	//util_waitus(1);   /* > 0.5 SCLK cycle */
	
	  CMT2300A_SendByte( addr | 0x80 );   /* MSB is 1 for read data flag to CMT */
	// turn over CMT_SDIO to input mode	
	CMT2300A_Turnto_sdioIN();
	  *pdat = CMT2300A_RecvByte();  // read one byte from CMT2300A
	
	CMT_SCLK_OUT_L();   util_wait98ns(11); 
	//util_waitus(1);   /* > 0.5 SCLK cycle */
	CMT_CSB_OUT_H();
	
	CMT_SDIO_OUT_H();  CMT_SDIO_IN();
	CMT_FCSB_OUT_H();
}

void CMT2300A_WriteFifo( const uint8_t *pbuf, uint16_t len )
{
  uint16_t i;
	
	CMT_FCSB_OUT_H();
	CMT_CSB_OUT_H();
	CMT_SCLK_OUT_L();
	
	// turn CMT_SDIO to output mode
	CMT2300A_Turnto_sdioOUT();
	for( i=0; i<len; i++ )
	{
	  CMT_FCSB_OUT_L();   util_wait98ns(22); 
		//util_waitus(2);   /* > 1 SCLK cycle */
		  CMT2300A_SendByte( pbuf[i] );
		CMT_SCLK_OUT_L();   util_wait98ns(33); 
		//util_waitus(3);   /* > 2us */
		CMT_FCSB_OUT_H();   util_wait98ns(55); 
		//util_waitus(5);   /* > 4us */
	}
	CMT_SDIO_IN();
	CMT_FCSB_OUT_H();
}
void CMT2300A_ReadFifo( uint8_t *pbuf, uint16_t len )
{
  uint16_t i;
	
	CMT_FCSB_OUT_H();
	CMT_CSB_OUT_H();
	CMT_SCLK_OUT_L();
	
	// turn CMT_SDIO to input mode
	CMT2300A_Turnto_sdioIN();
	for( i=0; i<len; i++ )
	{
	  CMT_FCSB_OUT_L();   util_wait98ns(22); 
		//util_waitus(2);   /* > 1 SCLK cycle */
		  pbuf[i] = CMT2300A_RecvByte();
		CMT_SCLK_OUT_L();   util_wait98ns(33); 
		//util_waitus(3);   /* > 2us */
		CMT_FCSB_OUT_H();   util_wait98ns(55); 
		//util_waitus(5);   /* > 4us */
	}
	CMT_SDIO_IN();
	CMT_FCSB_OUT_H();
}

//////////////////////////////////////////////////////////59
void CMT2300A_EnableWriteFifo( void )
{
  uint8_t tmp = 0xff;
	CMT2300A_ReadReg( CMT2300A_CUS_FIFO_CTL, &tmp ); 
  tmp |= CMT2300A_MASK_SPI_FIFO_RD_WR_SEL;
  tmp |= CMT2300A_MASK_FIFO_RX_TX_SEL;
  CMT2300A_WriteReg( CMT2300A_CUS_FIFO_CTL, tmp );
}
void CMT2300A_EnableReadFifo( void )
{
  uint8_t tmp = 0xff;
	CMT2300A_ReadReg( CMT2300A_CUS_FIFO_CTL, &tmp ); 
  tmp &= ~CMT2300A_MASK_SPI_FIFO_RD_WR_SEL; 
  tmp &= ~CMT2300A_MASK_FIFO_RX_TX_SEL;
  CMT2300A_WriteReg( CMT2300A_CUS_FIFO_CTL, tmp );
}

uint8_t CMT2300A_ClearTxFifo( void )
{
 	uint8_t tmp = 0xff;
	CMT2300A_ReadReg( CMT2300A_CUS_FIFO_FLAG, &tmp ); 
  CMT2300A_WriteReg( CMT2300A_CUS_FIFO_CLR, CMT2300A_MASK_FIFO_CLR_TX );
  return tmp;
}
uint8_t CMT2300A_ClearRxFifo( void )
{
 	uint8_t tmp = 0xff;
	CMT2300A_ReadReg( CMT2300A_CUS_FIFO_FLAG, &tmp ); 
  CMT2300A_WriteReg( CMT2300A_CUS_FIFO_CLR, CMT2300A_MASK_FIFO_CLR_RX );
  return tmp;
}

/*! ********************************************************
* @name    CMT2300A_EnableFifoMerge
* @desc    Enable FIFO merge.
* @param   bEnable(TRUE): use a single 64-byte FIFO for either Tx or Rx
*          bEnable(FALSE): use a 32-byte FIFO for Tx and another 32-byte FIFO for Rx(default)
* *********************************************************/
void CMT2300A_EnableFifoMerge( bool8_t bEnable )
{
    uint8_t tmp;  // = CMT2300A_ReadReg(CMT2300A_CUS_FIFO_CTL);

    CMT2300A_ReadReg( CMT2300A_CUS_FIFO_CTL, &tmp );

    if( bEnable )
        tmp |= CMT2300A_MASK_FIFO_MERGE_EN;
    else
        tmp &= ~CMT2300A_MASK_FIFO_MERGE_EN;

    //CMT2300A_WriteReg(CMT2300A_CUS_FIFO_CTL, tmp);
    CMT2300A_WriteReg( CMT2300A_CUS_FIFO_CTL, tmp );
}
void CMT2300A_RestoreFifo( void )
{
    CMT2300A_WriteReg( CMT2300A_CUS_FIFO_CLR, CMT2300A_MASK_FIFO_RESTORE );
}


void CMT2300A_ConfigGpio( uint8_t nGpioSel )
{
  CMT2300A_WriteReg( CMT2300A_CUS_IO_SEL, nGpioSel );
}

void CMT2300A_SetFrequencyChannel( uint8_t nChann )
{
    CMT2300A_WriteReg( CMT2300A_CUS_FREQ_CHNL, nChann );
}
void CMT2300A_SetFrequencyStep( uint8_t nOffset )
{
    CMT2300A_WriteReg( CMT2300A_CUS_FREQ_OFS, nOffset );
}

void CMT2300A_EnableLfosc( bool8_t bEnable )
{
  uint8_t tmp = 0xff;
  CMT2300A_ReadReg( CMT2300A_CUS_SYS2, &tmp );  
	
  if( bEnable ) {
    tmp |= CMT2300A_MASK_LFOSC_RECAL_EN;
    tmp |= CMT2300A_MASK_LFOSC_CAL1_EN;
    tmp |= CMT2300A_MASK_LFOSC_CAL2_EN;
  }
  else {
    tmp &= ~CMT2300A_MASK_LFOSC_RECAL_EN;
    tmp &= ~CMT2300A_MASK_LFOSC_CAL1_EN;
    tmp &= ~CMT2300A_MASK_LFOSC_CAL2_EN;
  }
  CMT2300A_WriteReg( CMT2300A_CUS_SYS2, tmp );
}

void CMT2300A_SetPayloadLength( uint16_t nLength )
{ 
  uint8_t tmp = 0xff;
	CMT2300A_ReadReg( CMT2300A_CUS_PKT14, &tmp ); 
  tmp &= ~CMT2300A_MASK_PAYLOAD_LENG_10_8;
  tmp |= (nLength >> 4) & CMT2300A_MASK_PAYLOAD_LENG_10_8;
  CMT2300A_WriteReg( CMT2300A_CUS_PKT14, tmp );
    
  tmp = nLength & CMT2300A_MASK_PAYLOAD_LENG_7_0;
  CMT2300A_WriteReg( CMT2300A_CUS_PKT15, tmp );
}

////////////////////////////////////////////////////53
void CMT2300A_EnableInterrupt( uint8_t nEnable )
{
  CMT2300A_WriteReg( CMT2300A_CUS_INT_EN, nEnable );
}
void CMT2300A_ConfigInterrupt( uint8_t nInt1Sel, uint8_t nInt2Sel )
{
	uint8_t temp;
  nInt1Sel &= CMT2300A_MASK_INT1_SEL;
  //nInt1Sel |= (~CMT2300A_MASK_INT1_SEL) & CMT2300A_ReadReg(CMT2300A_CUS_INT1_CTL);
  CMT2300A_ReadReg( CMT2300A_CUS_INT1_CTL, &temp ); 
	nInt1Sel |= (~CMT2300A_MASK_INT1_SEL) & temp;
	CMT2300A_WriteReg( CMT2300A_CUS_INT1_CTL, nInt1Sel );

  nInt2Sel &= CMT2300A_MASK_INT2_SEL;
  //nInt2Sel |= (~CMT2300A_MASK_INT2_SEL) & CMT2300A_ReadReg(CMT2300A_CUS_INT2_CTL);
  CMT2300A_ReadReg( CMT2300A_CUS_INT2_CTL, &temp ); 
	nInt1Sel |= (~CMT2300A_MASK_INT2_SEL) & temp;
	CMT2300A_WriteReg( CMT2300A_CUS_INT2_CTL, nInt2Sel );
}

uint8_t CMT2300A_ClearInterruptFlags( void )
{
  uint8_t nFlag1, nFlag2;
  uint8_t nClr1 = 0;
  uint8_t nClr2 = 0;
  uint8_t nRet  = 0;
  uint8_t nIntPolar;
    
    //nIntPolar = CMT2300A_ReadReg(CMT2300A_CUS_INT1_CTL);
	CMT2300A_ReadReg( CMT2300A_CUS_INT1_CTL, &nIntPolar ); 
  nIntPolar = (nIntPolar & CMT2300A_MASK_INT_POLAR) ? 0:1;

    //nFlag1 = CMT2300A_ReadReg(CMT2300A_CUS_INT_FLAG);
	CMT2300A_ReadReg( CMT2300A_CUS_INT_FLAG, &nFlag1 ); 
    //nFlag2 = CMT2300A_ReadReg(CMT2300A_CUS_INT_CLR1);
	CMT2300A_ReadReg( CMT2300A_CUS_INT_CLR1, &nFlag2 ); 
    
  if( nIntPolar ) {
    nFlag1 = ~nFlag1;   /* Interrupt flag active-low */
    nFlag2 = ~nFlag2;
  }

  if(CMT2300A_MASK_LBD_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_LBD_CLR;         /* Clear LBD_FLG */
  }

  if(CMT2300A_MASK_COL_ERR_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_PKT_DONE_CLR;    /* Clear COL_ERR_FLG by PKT_DONE_CLR */
  }

  if(CMT2300A_MASK_PKT_ERR_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_PKT_DONE_CLR;    /* Clear PKT_ERR_FLG by PKT_DONE_CLR */
  }

  if(CMT2300A_MASK_PREAM_OK_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_PREAM_OK_CLR;    /* Clear PREAM_OK_FLG */
    nRet  |= CMT2300A_MASK_PREAM_OK_FLG;    /* Return PREAM_OK_FLG */
  }

  if(CMT2300A_MASK_SYNC_OK_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_SYNC_OK_CLR;    /* Clear SYNC_OK_FLG */
    nRet  |= CMT2300A_MASK_SYNC_OK_FLG;    /* Return SYNC_OK_FLG */
  }

  if(CMT2300A_MASK_NODE_OK_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_NODE_OK_CLR;    /* Clear NODE_OK_FLG */
    nRet  |= CMT2300A_MASK_NODE_OK_FLG;    /* Return NODE_OK_FLG */
  }

  if(CMT2300A_MASK_CRC_OK_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_CRC_OK_CLR;    /* Clear CRC_OK_FLG */
    nRet  |= CMT2300A_MASK_CRC_OK_FLG;    /* Return CRC_OK_FLG */
  }

  if(CMT2300A_MASK_PKT_OK_FLG & nFlag1) {
    nClr2 |= CMT2300A_MASK_PKT_DONE_CLR;  /* Clear PKT_OK_FLG */
    nRet  |= CMT2300A_MASK_PKT_OK_FLG;    /* Return PKT_OK_FLG */
  }    

  if(CMT2300A_MASK_SL_TMO_FLG & nFlag2) {
    nClr1 |= CMT2300A_MASK_SL_TMO_CLR;    /* Clear SL_TMO_FLG */
    nRet  |= CMT2300A_MASK_SL_TMO_EN;     /* Return SL_TMO_FLG by SL_TMO_EN */
  }

  if(CMT2300A_MASK_RX_TMO_FLG & nFlag2) {
    nClr1 |= CMT2300A_MASK_RX_TMO_CLR;    /* Clear RX_TMO_FLG */
    nRet  |= CMT2300A_MASK_RX_TMO_EN;     /* Return RX_TMO_FLG by RX_TMO_EN */
  }

  if(CMT2300A_MASK_TX_DONE_FLG & nFlag2) {
    nClr1 |= CMT2300A_MASK_TX_DONE_CLR;   /* Clear TX_DONE_FLG */
    nRet  |= CMT2300A_MASK_TX_DONE_EN;    /* Return TX_DONE_FLG by TX_DONE_EN */
  }
    
  CMT2300A_WriteReg( CMT2300A_CUS_INT_CLR1, nClr1 );
  CMT2300A_WriteReg( CMT2300A_CUS_INT_CLR2, nClr2 );

  if(nIntPolar) 
	{
    nRet = ~nRet;       /* Interrupt flag active-low */
  }

  return nRet;
}

bool8_t CMT2300A_AutoSwitchStatus( uint8_t nGoCmd )
{
#ifdef ENABLE_AUTO_SWITCH_CHIP_STATUS
    u32 nBegTick = CMT2300A_GetTickCount();
    u8  nWaitStatus;
    
    switch( nGoCmd )
    {
      case CMT2300A_GO_SLEEP: nWaitStatus = CMT2300A_STA_SLEEP; break;
      case CMT2300A_GO_STBY : nWaitStatus = CMT2300A_STA_STBY ; break;
      case CMT2300A_GO_TFS  : nWaitStatus = CMT2300A_STA_TFS  ; break;
      case CMT2300A_GO_TX   : nWaitStatus = CMT2300A_STA_TX   ; break;
      case CMT2300A_GO_RFS  : nWaitStatus = CMT2300A_STA_RFS  ; break;
      case CMT2300A_GO_RX   : nWaitStatus = CMT2300A_STA_RX   ; break;
    }
    
    CMT2300A_WriteReg( CMT2300A_CUS_MODE_CTL, nGoCmd );
    
    while( CMT2300A_GetTickCount()-nBegTick < 10 )
    {
        wait_us(100);   //CMT2300A_DelayUs(100);
        
        if( nWaitStatus==CMT2300A_GetChipStatus() )
            return TRUE;
        
        if( CMT2300A_GO_TX==nGoCmd ) {
            wait_us(100);   //CMT2300A_DelayUs(100);
            
            if( CMT2300A_MASK_TX_DONE_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_CLR1))
                return TRUE;
        }
        
        if(CMT2300A_GO_RX==nGoCmd) {
            wait_us(100);   //CMT2300A_DelayUs(100);
            
            if( CMT2300A_MASK_PKT_OK_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_FLAG))
                return TRUE;
        }
    }
    
    return FALSE;
    
#else
    CMT2300A_WriteReg( CMT2300A_CUS_MODE_CTL, nGoCmd );
    return TRUE;
#endif
}

//////////////////////////////////////////////////////////59
bool8_t CMT2300A_IsExist( void )
{
  uint8_t back, dat;

   // back = CMT2300A_ReadReg(CMT2300A_CUS_PKT17);
	CMT2300A_ReadReg( CMT2300A_CUS_PKT17, &back ); 
  CMT2300A_WriteReg( CMT2300A_CUS_PKT17, 0xaa );

    //dat = CMT2300A_ReadReg(CMT2300A_CUS_PKT17);
	CMT2300A_ReadReg( CMT2300A_CUS_PKT17, &dat );
  CMT2300A_WriteReg( CMT2300A_CUS_PKT17, back );

  if( 0xaa==dat )
    return TRUE;
  else
    return FALSE;
}

bool8_t CMT2300A_SetStatus( uint8_t nGoCmd )
{
  CMT2300A_WriteReg( CMT2300A_CUS_MODE_CTL, nGoCmd );
	//util_waitus(73);  // 100us
	util_wait2us(47);
	return TRUE; 
}
uint8_t CMT2300A_GetChipStatus( void )
{
	uint8_t tempd=0xff;
	CMT2300A_ReadReg( CMT2300A_CUS_MODE_STA, &tempd );
	return ( tempd & CMT2300A_MASK_CHIP_MODE_STA );
   // return  CMT2300A_ReadReg(CMT2300A_CUS_MODE_STA) & CMT2300A_MASK_CHIP_MODE_STA;
}

void CMT2300A_SoftReset( void )
{
  CMT2300A_WriteReg( 0x7f, 0xff );
}



////////////////////////////////////////////////////53
void CMT2300A_ConfigA(void)
{
  uint8_t tmp = 0xff;

  CMT2300A_SoftReset();
  util_wait97us( 97 );
	//util_waitms(20);
    
    //CMT2300A_GoStby();
  CMT2300A_SetStatus( CMT2300A_GO_STBY );
	
    //tmp  = CMT2300A_ReadReg(CMT2300A_CUS_MODE_STA);
	CMT2300A_ReadReg( CMT2300A_CUS_MODE_STA, &tmp ); 
  tmp |= CMT2300A_MASK_CFG_RETAIN;         /* Enable CFG_RETAIN */
  tmp &= ~CMT2300A_MASK_RSTN_IN_EN;        /* Disable RSTN_IN */
  CMT2300A_WriteReg( CMT2300A_CUS_MODE_STA, tmp );

    //tmp  = CMT2300A_ReadReg(CMT2300A_CUS_EN_CTL);
	CMT2300A_ReadReg( CMT2300A_CUS_EN_CTL, &tmp );
  tmp |= CMT2300A_MASK_LOCKING_EN;         /* Enable LOCKING_EN */
  CMT2300A_WriteReg( CMT2300A_CUS_EN_CTL, tmp );
    
    CMT2300A_EnableLfosc(FALSE);             /* Disable LFOSC */

    CMT2300A_ClearInterruptFlags();
}

void CMT2300A_ConfigX( void )
{
  uint8_t tmp;
	// xosc_aac_code[2:0] = 2
  //tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
	CMT2300A_ReadReg( CMT2300A_CUS_CMT10, &tmp );
	tmp = (~0x07) & tmp;
  CMT2300A_WriteReg( CMT2300A_CUS_CMT10, tmp|0x02 );
}

void CMT2300A_ConfigZ( void )
{
#ifdef ENABLE_ANTENNA_SWITCH
    /* If you enable antenna switch, GPIO1/GPIO2 will output RX_ACTIVE/TX_ACTIVE,
       and it can't output INT1/INT2 via GPIO1/GPIO2 */
    CMT2300A_EnableAntennaSwitch(0);
    
#else
    /* Config chip's GPIO123 */
    CMT2300A_ConfigGpio(
         CMT2300A_GPIO1_SEL_INT1     /* INT1 > GPIO1 */
        |CMT2300A_GPIO2_SEL_INT2      /* INT2 > GPIO2 */
              //  |   CMT2300A_GPIO3_SEL_DOUT
        );   //CMT2300A_GPIO3_SEL_DOUT
    
    /* Config interrupt */
    CMT2300A_ConfigInterrupt(
           CMT2300A_INT_SEL_TX_DONE,   /* Config INT1 */
           CMT2300A_INT_SEL_PKT_DONE   /* Config INT2 */
        );
#endif

    /* Enable interrupt */
    CMT2300A_EnableInterrupt(
        CMT2300A_MASK_TX_DONE_EN  
      /* |CMT2300A_MASK_PREAM_OK_EN 
        |CMT2300A_MASK_SYNC_OK_EN  
        |CMT2300A_MASK_NODE_OK_EN  
        |CMT2300A_MASK_CRC_OK_EN   */
        |CMT2300A_MASK_PKT_DONE_EN
        );
    
    /* Disable low frequency OSC calibration */
    CMT2300A_EnableLfosc(FALSE);
    
    /* Use a single 64-byte FIFO for either Tx or Rx */
    //CMT2300A_EnableFifoMerge(TRUE);
    
    //CMT2300A_SetFifoThreshold(16);
    
    /* This is optional, only needed when using Rx fast frequency hopping */
    /* See AN142 and AN197 for details */
    //CMT2300A_SetAfcOvfTh(0x27);
    
    /* Go to sleep for configuration to take effect */
    //CMT2300A_GoSleep();
		CMT2300A_SetStatus( CMT2300A_GO_SLEEP );
}

bool8_t CMT2300A_ConfigRegBank( uint8_t base_addr, const uint8_t bank[], uint8_t len )
{
  uint8_t i;
  for( i=0; i<len; i++ )
  {  CMT2300A_WriteReg( i+base_addr, bank[i] );   }

  return TRUE;
}

bool8_t CMT2300A_GoTx( void )
{
    //return CMT2300A_AutoSwitchStatus(CMT2300A_GO_TX);
	CMT2300A_WriteReg( CMT2300A_CUS_MODE_CTL, CMT2300A_GO_TX );
	//util_waitus(300);    // maybe need read status reg 
	util_wait2us(123);
	return TRUE; 
}
bool8_t CMT2300A_GoRx( void )
{
    //return CMT2300A_AutoSwitchStatus(CMT2300A_GO_RX);
	CMT2300A_WriteReg( CMT2300A_CUS_MODE_CTL, CMT2300A_GO_RX );
	//util_waitus(400);    // maybe need read status reg 
	util_wait2us(183);
	return TRUE; 
}


/////////////////////////////////////////////////////////////////66 end of file




