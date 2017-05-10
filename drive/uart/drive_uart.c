/**
* @file 		gprs_uart.c
* @brief		�ṩΪgprsģ�����Ĵ��ڹ���.
* @details	
* @author		sundh
* @date		16-09-20
* @version	A001
* @par Copyright (c): 
* 		XXX??
* @par History:         
*	version: author, date, desc\n
* 	A001:sundh,16-09-20,��ʼ������
*/
#include "stdint.h"
#include "drive_uart.h"
#include "hardwareConfig.h"
#include "basis/sdhError.h"
#include <stdarg.h>
#include <string.h>
#include "sdhDef.h"
#include "mem/CiiMem.h"



#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
		
short		g_sendCount = 0;
short		g_sendLen = 0;
#endif
//�����жϳ����У��ҵ���Ӧ���豸
static driveUart	*devArry[ 3];


static void UartDma_Init( driveUart *self);

static int UartInit( driveUart *self, void *device, void *cfg)
{
	CfgUart_t *myCfg = ( CfgUart_t *)cfg;
	devArry[ myCfg->uartNum] = self;
	self->cfg = cfg;
	self->devUart = device;
	self->rxCache = calloc( 1, UART_RXCACHE_SIZE);
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	self->txCache = calloc( 1, UART_TXCACHE_SIZE);
#endif
	
	init_pingponfbuf( &self->ctl.pingpong, self->rxCache, UART_RXCACHE_SIZE, TURE);
	USART_Cmd( self->devUart, DISABLE);
	USART_DeInit( device);
	
	
	USART_Init( device, myCfg->cfguart);
	UartDma_Init( self);

	
	
	
	USART_ITConfig( device, USART_IT_RXNE, ENABLE);
	USART_ITConfig( device, USART_IT_IDLE, ENABLE);
	
	
#if SER485_SENDMODE == SENDMODE_DMA	
	USART_DMACmd( device, USART_DMAReq_Tx, ENABLE);  // ����DMA����
#endif
	USART_DMACmd( device, USART_DMAReq_Rx, ENABLE); // ����DMA����
	
	USART_Cmd( device, ENABLE);
	self->ctl.rx_block = 1;
	self->ctl.tx_block = 1;
	self->ctl.rx_waittime_ms = 100;
	self->ctl.tx_waittime_ms = 1000;
	
	return ERR_OK;
	
}


static int UartDeInit( driveUart *self)
{
	
	free( self->rxCache);
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	free( self->txCache);
#endif
	
	USART_Cmd( self->devUart, DISABLE);
	USART_DeInit( self->devUart);
	

	
	self->rxLedHdl = NULL;
	self->txLedHdl = NULL;
	self->rxIdp = NULL;
	self->txIdp = NULL;
	self->rxWait = NULL;
	self->txWait = NULL;
	self->rxPost = NULL;
	self->txPost = NULL;
	
	return ERR_OK;
	
}

/*!
**
**
** @param data 
** @param size 
** @return
**/
static int UartWrite( driveUart *self, void *buf, int wrLen)
{
	CfgUart_t *myCfg = ( CfgUart_t *)self->cfg;
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	int ret;
	char *sendbuf ;
#else
	int count = 0;
#endif
	if( buf == NULL)
		return ERR_BAD_PARAMETER;
	
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	if( wrLen  < UART_TXCACHE_SIZE)
	{
		memset( self->txCache, 0, UART_TXCACHE_SIZE);
		memcpy( self->txCache, buf, wrLen);
		sendbuf = self->txCache;
	
	}
	else
	{
		sendbuf = buf;
		
	}
#	if SER485_SENDMODE == SENDMODE_DMA		
	myCfg->dma->dma_tx_base->CMAR = (uint32_t)sendbuf;
	myCfg->dma->dma_tx_base->CNDTR = (uint16_t)wrLen; 
	DMA_Cmd( myCfg->dma->dma_tx_base, ENABLE);        //��ʼDMA����
#	elif SER485_SENDMODE == SENDMODE_INTR		
	
	self->ctl.intrSendingBuf = sendbuf;
	self->ctl.sendingCount = 1;
	self->ctl.sendingLen = size;
	
	USART_SendData( self->devUart, sendbuf[0]);
	USART_ITConfig( self->devUart, USART_IT_TXE, ENABLE);

#	endif
//	osDelay(1);
	if( self->ctl.tx_block)
	{
		if( self->txWait)
		{
			
			ret = self->txWait( self->ctl.tx_waittime_ms);
		}
		
		
		if ( ret > 0) 
		{
			while( USART_GetFlagStatus( self->devUart, USART_FLAG_TXE) == RESET){};
			return ERR_OK;
		}
		
		
		return ERR_DEV_TIMEOUT;
		
	}
	
#endif	//( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)

#if SER485_SENDMODE == SENDMODE_CPU		
	while( count < size)
	{
		USART_SendData( self->devUart, data[count]);
		while( USART_GetFlagStatus( self->devUart, USART_FLAG_TXE) == RESET){};
		count ++;
	}

#endif	
	return ERR_OK;
}



/*!
**
**
** @param data 
** @param size 
** @return
**/
static int UartRead( driveUart *self, void *buf, int rdLen)
{
	int  ret = 1;
	int len = rdLen;
	char *playloadbuf ;
	
	if( buf == NULL)
		return ERR_BAD_PARAMETER;
	

	
	if( self->rxWait)
	{
		
		ret = self->rxWait( self->ctl.rx_waittime_ms);
	}
	
	if( ret > 0)
	{
		if( len > self->ctl.recv_size)
			len = self->ctl.recv_size;
		playloadbuf = get_playloadbuf( &self->ctl.pingpong);
		memset( buf, 0, rdLen);
		memcpy( buf, playloadbuf, len);
		memset( playloadbuf, 0, len);
		free_playloadbuf( &self->ctl.pingpong);
//		LED_com->turnon(LED_com);
//		if( T485Rxirq_cb != NULL && len)
//			T485Rxirq_cb->cb( NULL,  T485Rxirq_cb->arg);
		self->ctl.recv_size = 0;
		return len;
	}
	
	return 0;
}

//�����ڵĻ����������
//���ص�ǰ��������ݳ���
static int UartTakeUpPlayloadBuf( driveUart *self, void **data)
{
	int  ret = 1;
	int len = 0;
	char *playloadbuf ;

	if( self->rxWait)
	{
		
		ret = self->rxWait( self->ctl.rx_waittime_ms);
	}
	
	if( ret > 0)
	{
		
		len = self->ctl.recv_size;
		playloadbuf = get_playloadbuf( &self->ctl.pingpong);
		self->ctl.recv_size = 0;
		*data = playloadbuf;
		return len;
	}
	
	return 0;
}

int UartGiveBackPlayloadBuf( driveUart *self, void *data)
{
	free_playloadbuf( &self->ctl.pingpong);
	return 0;
}


/*!
**
**
** @param size
**
** @return
**/
static void UartIoctol( driveUart *self, int cmd, ...)
{
	int int_data;
	va_list arg_ptr; 
	va_start(arg_ptr, cmd); 
	
	switch(cmd)
	{
		case S485_UART_CMD_SET_TXBLOCK:
			self->ctl.tx_block = 1;
			break;
		case S485_UART_CMD_CLR_TXBLOCK:
			self->ctl.tx_block = 0;
			break;
		case S485_UART_CMD_SET_RXBLOCK:
			self->ctl.rx_block = 1;
			break;
		case S485_UART_CMD_CLR_RXBLOCK:
			self->ctl.rx_block = 0;
			self->ctl.rx_waittime_ms = 0;
			break;
		case S485UART_SET_TXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			self->ctl.tx_waittime_ms = int_data;
			break;
		case S485UART_SET_RXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			self->ctl.rx_waittime_ms = int_data;
			break;
		
		
		
		default: break;
		
	}
}




static int UartTest( driveUart *self, void *buf, int size)
{
	char *pp = NULL;
    int len;
	
	
	strcpy( buf, "Serial 485 test\n" );
	self->ioctol( self, S485UART_SET_TXWAITTIME_MS, 10);
	self->ioctol( self, S485UART_SET_RXWAITTIME_MS, 10);
	
	self->write( self, buf, strlen(buf));
	
	while(1)
	{
		len = self->read( self, buf, size);
		pp = strstr((const char*)buf,"OK");
		if(pp)
			return ERR_OK;
		
		if( len > 0)
			self->write( self, buf, len);
	}
	
}


static void UartSetLedHdl( driveUart *self, int rxOrTx, ledHdl hdl)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxLedHdl = hdl;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txLedHdl = hdl;
	} 
	
	
}

static void UartSetIdp( driveUart *self, int rxOrTx, uartIdp idp)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxIdp = idp;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txIdp = idp;
	} 
	
	
}
static void UartSetWaitSem( driveUart *self, int rxOrTx, waitSem wait)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxWait = wait;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txWait = wait;
	} 
	
	
}
static void UartSetPostSem( driveUart *self, int rxOrTx, postSem post)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxPost = post;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txPost = post;
	} 
	
}

/*! gprs uart dma Configuration*/
void UartDma_Init( driveUart *self)
{
	CfgUart_t *myCfg = ( CfgUart_t *)self->cfg;
	USART_TypeDef *devUart = ( USART_TypeDef *)self->devUart;
	
	DMA_InitTypeDef DMA_InitStructure;	
	short rxbuflen;
	char *rxbuf;

    
//=DMA_Configuration==============================================================================//	
/*--- UART_Tx_DMA_Channel DMA Config ---*/
#if SER485_SENDMODE == SENDMODE_DMA	

    DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE);                           // �ر�DMA
    DMA_DeInit( myCfg->dma->dma_tx_base);                                 // �ָ���ʼֵ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&devUart->DR);// �����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)self->txCache;        
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // ���ڴ浽����
    DMA_InitStructure.DMA_BufferSize = UART_TXCACHE_SIZE;                    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // �����ַ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // �ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݿ��1B
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // �ڴ��ַ���1B
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // ���δ���ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;                 // ���ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // �ر��ڴ浽�ڴ�ģʽ
    DMA_Init( myCfg->dma->dma_tx_base, &DMA_InitStructure);               // 
    
	DMA_ClearFlag( myCfg->dma->dma_tx_flag );                                 // �����־
	DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE); 												// �ر�DMA
    DMA_ITConfig( myCfg->dma->dma_tx_base, DMA_IT_TC, ENABLE);            // ����������ж�

 #endif  

/*--- UART_Rx_DMA_Channel DMA Config ---*/

 
	switch_receivebuf( &self->ctl.pingpong, &rxbuf, &rxbuflen);
    DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE);                           
    DMA_DeInit( myCfg->dma->dma_rx_base);                                 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&devUart->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxbuf;         
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     
    DMA_InitStructure.DMA_BufferSize = rxbuflen;                     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;                 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            
    DMA_Init( myCfg->dma->dma_rx_base, &DMA_InitStructure);               
    DMA_ClearFlag( myCfg->dma->dma_rx_flag);      
//	DMA_ITConfig(DMA_s485_usart.dma_rx_base, DMA_IT_TC, ENABLE); 	 // ����������ж�

    DMA_Cmd( myCfg->dma->dma_rx_base, ENABLE);                            

   

}

//�жϴ�����������Դ�봮�ڵİ������ѡ���豸
void DMA1_Channel4_IRQHandler(void)
{
	driveUart	*thisDev = devArry[0];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
    if(DMA_GetITStatus(DMA1_FLAG_TC4))
    {
			
			DMA_ClearFlag( myCfg->dma->dma_tx_flag);         // �����־
			DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE);   // �ر�DMAͨ��
			if( thisDev->txPost)
				thisDev->txPost();
		
    }
}

//dma�����������Ժ�,�л����ջ���
//DMA�����ж�
void DMA1_Channel5_IRQHandler(void)
{
	short rxbuflen;
	char *rxbuf;
	driveUart	*thisDev = devArry[0];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
	
	
    if(DMA_GetITStatus(DMA1_FLAG_TC5))
    {
		DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE); 
        DMA_ClearFlag( myCfg->dma->dma_rx_flag);         // �����־
		thisDev->ctl.recv_size = get_loadbuflen( &thisDev->ctl.pingpong)  - \
		DMA_GetCurrDataCounter(  myCfg->dma->dma_rx_base); //��ý��յ����ֽ�
		
		switch_receivebuf( &thisDev->ctl.pingpong, &rxbuf, &rxbuflen);
		myCfg->dma->dma_rx_base->CMAR = (uint32_t)rxbuf;
		myCfg->dma->dma_rx_base->CNDTR = rxbuflen;
		DMA_Cmd(  myCfg->dma->dma_rx_base, ENABLE);
		if( thisDev->rxLedHdl)
			thisDev->rxLedHdl();
		if( thisDev->rxPost)
			thisDev->rxPost();
    }
}

void USART1_IRQHandler(void)
{
	uint8_t clear_idle = clear_idle;
	short rxbuflen;
	char *rxbuf;
	driveUart	*thisDev = devArry[0];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
	USART_TypeDef *devUart = ( USART_TypeDef *)thisDev->devUart;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  // �����ж�
	{
		
		DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE);       // �ر�DMA
		DMA_ClearFlag(  myCfg->dma->dma_rx_flag );           // ���DMA��־
		thisDev->ctl.recv_size = get_loadbuflen( &thisDev->ctl.pingpong)  - \
		DMA_GetCurrDataCounter(  myCfg->dma->dma_rx_base); //��ý��յ����ֽ�
//		S485_uart_ctl.recv_size = S485_UART_BUF_LEN - DMA_GetCurrDataCounter(DMA_s485_usart.dma_rx_base); //��ý��յ����ֽ�
		
		switch_receivebuf( &thisDev->ctl.pingpong, &rxbuf, &rxbuflen);
		myCfg->dma->dma_rx_base->CMAR = (uint32_t)rxbuf;
		myCfg->dma->dma_rx_base->CNDTR = rxbuflen;
		DMA_Cmd(  myCfg->dma->dma_rx_base, ENABLE);
		
		clear_idle = devUart->SR;
		clear_idle = devUart->DR;
		USART_ReceiveData( USART1 ); // Clear IDLE interrupt flag bit
		if( thisDev->rxLedHdl)
			thisDev->rxLedHdl();
		if( thisDev->rxPost)
			thisDev->rxPost();
	}
#if SER485_SENDMODE == SENDMODE_INTR	
	if(USART_GetITStatus(USART1, USART_IT_TXE) == SET)  // �����ж�
	{
		thisDev->ctl.sendingCount ++;
			
	
		//������ɺ�رշ�չ�жϣ����ⷢ�ͼĴ������˾ͻ�����ж�
		if( thisDev->ctl.sendingCount >= thisDev->ctl.sendingLen)
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			if( thisDev->txPost)
				thisDev->txPost();
		}			
		else
			USART_SendData(USART1, thisDev->ctl.intrSendingBuf[ thisDev->ctl.sendingCount] );
		
		
		
	}
	
#endif

}


//�жϴ�����������Դ�봮�ڵİ������ѡ���豸
void DMA1_Channel7_IRQHandler(void)
{
	driveUart	*thisDev = devArry[1];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
    if(DMA_GetITStatus(DMA1_FLAG_TC7))
    {
			
			DMA_ClearFlag( myCfg->dma->dma_tx_flag);         // �����־
			DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE);   // �ر�DMAͨ��
			if( thisDev->txPost)
				thisDev->txPost();
		
    }
}

//dma�����������Ժ�,�л����ջ���
void DMA1_Channel6_IRQHandler(void)
{
	short rxbuflen;
	char *rxbuf;
	driveUart	*thisDev = devArry[1];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
	
	
    if(DMA_GetITStatus(DMA1_FLAG_TC6))
    {
		DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE); 
        DMA_ClearFlag( myCfg->dma->dma_rx_flag);         // �����־
		thisDev->ctl.recv_size = get_loadbuflen( &thisDev->ctl.pingpong)  - \
		DMA_GetCurrDataCounter(  myCfg->dma->dma_rx_base); //��ý��յ����ֽ�
		
		switch_receivebuf( &thisDev->ctl.pingpong, &rxbuf, &rxbuflen);
		myCfg->dma->dma_rx_base->CMAR = (uint32_t)rxbuf;
		myCfg->dma->dma_rx_base->CNDTR = rxbuflen;
		DMA_Cmd(  myCfg->dma->dma_rx_base, ENABLE);
		if( thisDev->rxLedHdl)
			thisDev->rxLedHdl();
		if( thisDev->rxPost)
			thisDev->rxPost();
    }
}

void USART2_IRQHandler(void)
{
	uint8_t clear_idle = clear_idle;
	short rxbuflen;
	char *rxbuf;
	driveUart	*thisDev = devArry[1];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
	USART_TypeDef *devUart = ( USART_TypeDef *)thisDev->devUart;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  // �����ж�
	{
		
		DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE);       // �ر�DMA
		DMA_ClearFlag(  myCfg->dma->dma_rx_flag );           // ���DMA��־
		thisDev->ctl.recv_size = get_loadbuflen( &thisDev->ctl.pingpong)  - \
		DMA_GetCurrDataCounter(  myCfg->dma->dma_rx_base); //��ý��յ����ֽ�
//		S485_uart_ctl.recv_size = S485_UART_BUF_LEN - DMA_GetCurrDataCounter(DMA_s485_usart.dma_rx_base); //��ý��յ����ֽ�
		
		switch_receivebuf( &thisDev->ctl.pingpong, &rxbuf, &rxbuflen);
		myCfg->dma->dma_rx_base->CMAR = (uint32_t)rxbuf;
		myCfg->dma->dma_rx_base->CNDTR = rxbuflen;
		DMA_Cmd(  myCfg->dma->dma_rx_base, ENABLE);
		
		clear_idle = devUart->SR;
		clear_idle = devUart->DR;
		USART_ReceiveData( USART2 ); // Clear IDLE interrupt flag bit
		if( thisDev->rxLedHdl)
			thisDev->rxLedHdl();
		if( thisDev->rxPost)
			thisDev->rxPost();
	}
#if SER485_SENDMODE == SENDMODE_INTR	
	if(USART_GetITStatus(USART2, USART_IT_TXE) == SET)  // �����ж�
	{
		thisDev->ctl.sendingCount ++;
			
	
		//������ɺ�رշ�չ�жϣ����ⷢ�ͼĴ������˾ͻ�����ж�
		if( thisDev->ctl.sendingCount >= thisDev->ctl.sendingLen)
		{
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			if( thisDev->txPost)
				thisDev->txPost();
		}			
		else
			USART_SendData(USART2, thisDev->ctl.intrSendingBuf[ thisDev->ctl.sendingCount] );
		
		
		
	}
	
#endif

}




CTOR( driveUart)
FUNCTION_SETTING( init, UartInit);
FUNCTION_SETTING( deInit, UartDeInit);
FUNCTION_SETTING( read, UartRead);
FUNCTION_SETTING( takeUpPlayloadBuf, UartTakeUpPlayloadBuf);
FUNCTION_SETTING( giveBackPlayloadBuf, UartGiveBackPlayloadBuf);
FUNCTION_SETTING( write, UartWrite);
FUNCTION_SETTING( ioctol, UartIoctol);
FUNCTION_SETTING( setLedHdl, UartSetLedHdl);
FUNCTION_SETTING( setIdp, UartSetIdp);
FUNCTION_SETTING( setWaitSem, UartSetWaitSem);
FUNCTION_SETTING( setPostSem, UartSetPostSem);
FUNCTION_SETTING( test, UartTest);
END_CTOR

