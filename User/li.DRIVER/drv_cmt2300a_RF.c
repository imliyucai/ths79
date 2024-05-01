/**
  ******************************************************************************
  * @file      drv_cmt2300a_RF.c
  * @author    liyc
  * @version   V1.0
  * @date      2024-4-10
  * @brief     RSD79 board bsp
	*            driver source file of rf_chip cmt2300a 
  ******************************************************************************
  **/
	

#include "./drv_cmt2300a.h"
#include  "../../User.stdlib/li.BSP/Inc/bsp_cmt2300a.h"

#include  "../../User/li.APP/util_time.h"


const uint8_t g_cmt2300aCmtBank[ CMT2300A_CMT_BANK_SIZE ] = {
    0x00,
    0x66,
    0xEC,
    0x1C,
    0xF0,
    0x80,
    0x14,
    0x08,
    0x91,
    0x02,
    0x02,
    0xd0,   /*0xc0*/
};

/* [System Bank] */
const uint8_t g_cmt2300aSystemBank[ CMT2300A_SYSTEM_BANK_SIZE ] = {
    0xAE,
    0xE0,
    0x35,
    0x00,
    0x00,
    0xF4,
    0x10,
    0xE2,
    0x42,
    0x20,
    0x00,
    0x81,
};

/* [Frequency Bank] */
const uint8_t g_cmt2300aFrequencyBank[ CMT2300A_FREQUENCY_BANK_SIZE ] = {
    0x42,
    0x71,
    0xCE,
    0x1C,
    0x42,
    0x5B,
    0x1C,
    0x1C,
};

/* [Data Rate Bank] */
const uint8_t g_cmt2300aDataRateBank[ CMT2300A_DATA_RATE_BANK_SIZE ] = {
    0x19,  /*0x21*/
    0x0c,  /*0x10*/
    0x00,  /*0x10*/
    0xbb,  /*0xaa*/
    0xe9,  /*0xe1*/
    0xbd,  /*0xac*/
    0x19,
    0x05,  /*0x0a*/
    0xdF,  /*0x9f*/
    0x26,
    0x29,
    0x29,
    0xC0,
    0xa2,  /*0x7a*/
    0x54,  /*0x3f*/
    0x53,
    0x00,
    0x00,
    0xB4,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,
};

/* [Baseband Bank] */
const uint8_t g_cmt2300aBasebandBank[ CMT2300A_BASEBAND_BANK_SIZE ] = {
    0x12,
    0x08,   /*0x06*/
    0x00,
    0xaa,
    0x02,   /*0x04*/
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,   /*0xd5*/
    0xd4,   /*0xde*/
    0x2d,   /*0x21*/
    0x00,
    0x14,   /*0x0f*/
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x60,
    0xFF,
    0x00,
    0x00,
    0x1F,
    0x10,
};

/* [Tx Bank] */
const uint8_t g_cmt2300aTxBank[ CMT2300A_TX_BANK_SIZE ] = {
    0x50,
    0x9a,
    0x0c,
    0x00,
    0x86,   /*0x64*/
    0xd0,   /*0xd0*/
    0x00,
    0x1f,   /*0x37*/
    0x04,   /*0x0a*/
    0x3F,
    0x7F,
};




//////////////////////////////////////////////47
void RF433M_Init( void )
{
  
	CMT2300A_ConfigA();
    
  CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR       , g_cmt2300aCmtBank       , CMT2300A_CMT_BANK_SIZE       );
  CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR    , g_cmt2300aSystemBank    , CMT2300A_SYSTEM_BANK_SIZE    );
  CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , g_cmt2300aFrequencyBank , CMT2300A_FREQUENCY_BANK_SIZE );
  CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , g_cmt2300aDataRateBank  , CMT2300A_DATA_RATE_BANK_SIZE );
  CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR  , g_cmt2300aBasebandBank  , CMT2300A_BASEBAND_BANK_SIZE  );
  CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , g_cmt2300aTxBank        , CMT2300A_TX_BANK_SIZE        );
    
 	CMT2300A_ConfigX();   
	CMT2300A_ConfigZ();
}

void CMT2300A_SendData( uint8_t *pdata, uint16_t len )
{
	
  CMT2300A_SetStatus( CMT2300A_GO_STBY );  
	    util_wait97us( 43 );
	    //util_waitms(10);
	CMT2300A_ClearTxFifo();
	CMT2300A_EnableWriteFifo();
		
	CMT2300A_WriteFifo( pdata, len );
	CMT2300A_GoTx();
	   util_wait3ms( 97 );  // 1200bps. (20byte+10byte)*8=240bit -> 200ms 
	   util_wait3ms(81);   // why so long time? must patch
    	//util_waitms(317);   // must enough time wait 317ms to send data over........

	//while( CMT_IO1_IN() != Bit_SET ) 
	//{
	  // wait.... send over
	//}
	//CMT2300A_ClearInterruptFlags();
  //CMT2300A_SetStatus( CMT2300A_GO_SLEEP ); 
	
}




bool8_t CMT2300A_RecieveData( uint8_t *pdata, uint16_t len ) 
{
  
	CMT2300A_SetStatus( CMT2300A_GO_STBY );  
	    util_wait97us( 47 );
	//util_waitms(10);
	CMT2300A_GoRx();
       //CMT2300A_EnableReadFifo(); 
     util_wait3ms( 173 );
	   //util_waitms(319);   // must enough time wait 319ms to receive data ok........
	
	
  //if ( RF433M_GetStatus() == RF_STATE_RX_DONE )
  if( CMT_IO2_IN() )
  {  
		CMT2300A_EnableReadFifo(); 
    CMT2300A_ReadFifo( pdata, len );
		
		CMT2300A_ClearInterruptFlags();
    CMT2300A_ClearRxFifo();
    return TRUE;          
  }
	return FALSE;
}


//// RF 433MHz Tx Rx Handlers ///////////////////////

static enumRFStatus g_nNextRFState = RF_STATE_IDLE;
static uint8_t* g_pRxBuffer = 0;  //NULL;
static uint8_t* g_pTxBuffer = 0;  //NULL;
static uint16_t g_nRxLength = 0;
static uint16_t g_nTxLength = 0;

#define INFINITE 0xFFFFFFFF
static uint32_t g_nRxTimeout = INFINITE;
static uint32_t g_nTxTimeout = INFINITE;
static uint32_t g_nRxTimeCount = 0;
static uint32_t g_nTxTimeCount = 0;

static uint8_t g_nInterrutFlags = 0;

void RF433M_SetStatus( enumRFStatus nStatus )
{
  g_nNextRFState = nStatus;
}

enumRFStatus RF433M_GetStatus( void )
{
  return g_nNextRFState;
}

uint8_t RF433M_GetInterruptFlags( void )
{
  return g_nInterrutFlags;
}

void RF433M_StartRx( uint8_t buf[], uint16_t len, uint32_t timeout )
{
  g_pRxBuffer = buf;
  g_nRxLength = len;
  g_nRxTimeout = timeout;
    
 // memset( g_pRxBuffer, 0, g_nRxLength );
    
  g_nNextRFState = RF_STATE_RX_START;  
}

void RF433M_StartTx( uint8_t buf[], uint16_t len, uint32_t timeout )
{
  g_pTxBuffer = buf;
  g_nTxLength = len;
  g_nTxTimeout = timeout;
    
  g_nNextRFState = RF_STATE_TX_START;  
}


///////////////////////////////////////////////////////////
// the RF433M radio FSM based on g_nNextRFState
enumRFResult  RF433M_Processing( void )
{
  enumRFResult nRes = RF_BUSY;
    
  switch( g_nNextRFState ) 
  {
    case RF_STATE_IDLE:
    {
      nRes = RF_IDLE;
      break;
    }
    
    case RF_STATE_RX_START:
    {
      CMT2300A_SetStatus( CMT2300A_GO_STBY );
      CMT2300A_ClearInterruptFlags();
        
      /* Must clear FIFO after enable SPI to read or write the FIFO */
      CMT2300A_EnableReadFifo();
      CMT2300A_ClearRxFifo();
        
      if( FALSE == CMT2300A_GoRx() )
        g_nNextRFState = RF_STATE_ERROR;
      else
        g_nNextRFState = RF_STATE_RX_WAIT;
        
//      g_nRxTimeCount = CMT2300A_GetTickCount();
g_nRxTimeCount = 100;
			
      break;
    }
    
    case RF_STATE_RX_WAIT:
    {
#ifdef ENABLE_ANTENNA_SWITCH
      if(CMT2300A_MASK_PKT_OK_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_FLAG))  /* Read PKT_OK flag */
#else
      //if( CMT2300A_ReadGpio2() )  /* Read INT2, PKT_OK */
			if( CMT_IO2_IN() )
#endif
      {
        g_nNextRFState = RF_STATE_RX_DONE;
      }
        
//      if( (INFINITE != g_nRxTimeout) && ((g_nSysTickCount-g_nRxTimeCount) > g_nRxTimeout) )
//        g_nNextRFState = RF_STATE_RX_TIMEOUT;
g_nRxTimeCount--;   
if( g_nRxTimeCount==0 )	  g_nNextRFState = RF_STATE_RX_TIMEOUT;
			
      break;
    }
    
    case RF_STATE_RX_DONE:
    {
      CMT2300A_SetStatus( CMT2300A_GO_STBY );
      /* The length need be smaller than 32 */
      CMT2300A_ReadFifo( g_pRxBuffer, g_nRxLength );

      g_nInterrutFlags = CMT2300A_ClearInterruptFlags();
            
      CMT2300A_SetStatus( CMT2300A_GO_SLEEP );  
			
      g_nNextRFState = RF_STATE_IDLE;
      nRes = RF_RX_DONE;
      break;
    }
    
    case RF_STATE_RX_TIMEOUT:
    {
      CMT2300A_SetStatus( CMT2300A_GO_SLEEP ); 
			
      g_nNextRFState = RF_STATE_IDLE;
      nRes = RF_RX_TIMEOUT;
      break;
    }
    
    case RF_STATE_TX_START:
    {
      CMT2300A_SetStatus( CMT2300A_GO_STBY );
      CMT2300A_ClearInterruptFlags();
        
      /* Must clear FIFO after enable SPI to read or write the FIFO */
      CMT2300A_EnableWriteFifo();
      CMT2300A_ClearTxFifo();
        
      /* The length need be smaller than 32 */
      CMT2300A_WriteFifo( g_pTxBuffer, g_nTxLength );
        
//      if( 0==(CMT2300A_MASK_TX_FIFO_NMTY_FLG & CMT2300A_ReadReg(CMT2300A_CUS_FIFO_FLAG)) )
//        g_nNextRFState = RF_STATE_ERROR;

      if( FALSE == CMT2300A_GoTx() )
        g_nNextRFState = RF_STATE_ERROR;
      else
        g_nNextRFState = RF_STATE_TX_WAIT;
        
//      g_nTxTimeCount = CMT2300A_GetTickCount();
g_nTxTimeCount = 100;        
			
      break;
    }
        
    case RF_STATE_TX_WAIT:
    {
#ifdef ENABLE_ANTENNA_SWITCH
      if(CMT2300A_MASK_TX_DONE_FLG & CMT2300A_ReadReg(CMT2300A_CUS_INT_CLR1))  /* Read TX_DONE flag */
#else
      //if( CMT2300A_ReadGpio1() )  /* Read INT1, TX_DONE     CMT_GPIO1_IN()*/  
      if( CMT_IO1_IN() ) 
#endif
      {
        g_nNextRFState = RF_STATE_TX_DONE;
      }
        
//      if( (INFINITE != g_nTxTimeout) && ((g_nSysTickCount-g_nTxTimeCount) > g_nTxTimeout) )
//        g_nNextRFState = RF_STATE_TX_TIMEOUT;
g_nTxTimeCount--;
if( g_nTxTimeCount == 0 )    g_nNextRFState = RF_STATE_TX_TIMEOUT;
			
      break;
    }
            
    case RF_STATE_TX_DONE:
    {
      CMT2300A_ClearInterruptFlags();
      CMT2300A_SetStatus( CMT2300A_GO_SLEEP ); 
			
      g_nNextRFState = RF_STATE_IDLE;
      nRes = RF_TX_DONE;
      break;
    }
    
    case RF_STATE_TX_TIMEOUT:
    {
      CMT2300A_SetStatus( CMT2300A_GO_SLEEP ); 
			
      g_nNextRFState = RF_STATE_IDLE;
      nRes = RF_TX_TIMEOUT;
      break;
    }
    
    case RF_STATE_ERROR:
    {
      CMT2300A_SoftReset();
        //CMT2300A_DelayMs(20);
      //wait_ms(20); 
			util_waitms( 20 );
        
			CMT2300A_SetStatus( CMT2300A_GO_STBY );
      RF433M_Init();
        
      g_nNextRFState = RF_STATE_IDLE;
      nRes = RF_ERROR;
      break;
    }
    
    default:
      break;
    }
    
    return nRes;
}



/////////////////////////////////////////////////////////////////66 end of file

