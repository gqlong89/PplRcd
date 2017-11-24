//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//
#include "Ch376.h"
#include "os/os_depend.h"
#include "device.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define	xReadCH376Status( )		( CH376_CMD_PORT )  // ��CH376��״̬
#define xWriteCH376Data(d)		(CH376_DATA_PORT = d)
#define xReadCH376Data()		(CH376_DATA_PORT)
#define	xEndCH376Cmd( )			//����CH376����,������SPI�ӿڷ�ʽ

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

uint8 DataBuff[64];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static void DelayMs(unsigned long time);
static void Delay10us(unsigned long time);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{






//time = 1��1ms����ʱ
//time = 10��4.2ms����ʱ
//time = 20��8.2ms����ʱ
static void DelayMs(unsigned long time)
{
	unsigned long i,j;
	
	for(i=0; i<2800; i++)
		for(j=0; j<time; j++);
}	


//time = 1��11us����ʱ
static void Delay10us(unsigned long time)
{
	unsigned long i,j;
	
	for(i=0; i<30; i++)
		for(j=0; j<time; j++);
}



//���usb��������
//��������ʱ��USBOCA�����ͣ���Ӧ��������USBENA���ɵ͵�ƽ
void Test_USBOCA(void)
{
	if(!(IO0PIN & USB_OCA))
	{
		SET_CH376ENA_LOW;
		DelayMs(100);
	}
}


/*
**************************************************************************
* �������ƣ�HRst_Ch376
* �����������
* �����������
* ����������Ӳ����λ
**************************************************************************
*/
void HRst_Ch376(void)
{
	
	SET_CH376RST_HIGH;
	DelayMs(100);
	
	SET_CH376RST_LOW;
	DelayMs(100);

	SET_CH376ENA_HIGH;
	DelayMs(100);

}



void xWriteCH376Cmd(uint8 mCmd)
{
	uint8 i,res;

	CH376_CMD_PORT = mCmd;
	for (i=0;i<10;i++)
	{
		//���״̬�˿ڵı�־λ��λ4��æ��־������Ч
		res = xReadCH376Status();
		if (( res & PARA_STATE_BUSY) == 0)
		{
			break;
		}
	}
}



//��ʼ��CH376
uint8 mInitCH376Host(void)
{
	uint8 res;

	//���ͨѶ�ӿ�
	xWriteCH376Cmd(CMD11_CHECK_EXIST);
	DelayMs(100);
	xWriteCH376Data(0x65);
	DelayMs(100);
	res = xReadCH376Data();
	if (res != 0x9A)
	{
		return (ERR_USB_UNKNOWN);
	}


	//��ȡоƬ���̼��汾
	xWriteCH376Cmd(CMD01_GET_IC_VER);
	DelayMs(1000);
	res = xReadCH376Data();
	if (res != 0x41)
	{
//		return (ERR_USB_UNKNOWN);
		res = 0x41;
	}


	//����USB����ģʽ
	xWriteCH376Cmd(CMD11_SET_USB_MODE);
	xWriteCH376Data(0x06);	//�����õ�������ʽ�����Զ�����SOF��
	DelayMs(100);
	res = xReadCH376Data();
	if (res == CMD_RET_SUCCESS)
	{
		return USB_INT_SUCCESS;
	}
	else
	{
		return ERR_USB_UNKNOWN;
	}
}


//��ѯCH376�ж�(INT#�͵�ƽ)
uint8 Query376Interrupt(void)
{
	//������˿ڶ�ȡ�ӿ�״̬��λ7���жϱ�־������Ч
	//return (xReadCH376Status() & PARA_STATE_INTB ? FALSE : TRUE ); 
	//if ((IO0PIN >> 20) & 0x01 == 0)
	if(IO0PIN & 0x00100000)
	{
		return FALSE;
	}
	else
		return TRUE;
}


// ��ȡ�ж�״̬��ȡ���ж����� 

uint8 CH376GetIntStatus(void)
{
	uint8 s;

	xWriteCH376Cmd(CMD01_GET_STATUS);
	s = xReadCH376Data();
	xEndCH376Cmd();
	return (s);
}


uint8 Wait376Interrupt( void )  
{
	uint32	i;
	
	for ( i = 0; i < 1000000; i ++ )
	//while(1)
	{
		if ( Query376Interrupt( ) )
			return( CH376GetIntStatus( ) );

	}
	return( ERR_USB_UNKNOWN );
}


//�����������,�ȴ��ж� 
uint8	CH376SendCmdWaitInt( uint8 mCmd )  
{
	xWriteCH376Cmd( mCmd );
	xEndCH376Cmd( );
	return( Wait376Interrupt( ) );
}

// ���U���Ƿ�����,��֧��SD�� 
uint8 Ch376DiskConnect(void)
{
	
	if ( Query376Interrupt( ) )
		CH376GetIntStatus( );  // ��⵽�ж� 
	return( CH376SendCmdWaitInt( CMD0H_DISK_CONNECT ) );
	
}


// ��ʼ�����̲����Դ����Ƿ����
uint8 CH376DiskMount( void ) 
{
	return( CH376SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
}



// �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�,���س��� 
uint8	CH376ReadBlock( uint8 *buf )
{
	uint8	s, l;

	xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
	s = l = xReadCH376Data( );  /* ���� */
	if ( l ) {
		do {
			*buf = xReadCH376Data( );
			DelayMs(5);
			buf ++;
		} while ( -- l );
	}
	xEndCH376Cmd( );
	return( s );
}





uint8	CH376ReadVar8( uint8 var )  /* ��CH376оƬ�ڲ���8λ���� */
{
	uint8	c0;

	xWriteCH376Cmd( CMD11_READ_VAR8 );
	xWriteCH376Data( var );
	c0 = xReadCH376Data( );
	xEndCH376Cmd( );
	return( c0 );
}


//��ȡ���̺��ļ�ϵͳ�Ĺ���״̬
uint8	Ch376GetDiskStatus(void)
{
	return (CH376ReadVar8(VAR_DISK_STATUS));
}


// ��USB�����˵�ķ��ͻ�����д�����ݿ� 
void	CH376WriteHostBlock( uint8 *buf, uint8 len )
{
	xWriteCH376Cmd( CMD10_WR_HOST_DATA );
	xWriteCH376Data( len );  // ���� 
	if ( len )
	{
		do
		{
			xWriteCH376Data( *buf );
			//DelayMs(2);
			buf ++;
		} while ( -- len );
	}
	xEndCH376Cmd( );
}


// ���USB�洢������ 
uint8	CH376DiskReqSense( void )  
{
	uint8	s;
	DelayMs( 50 );
	s = CH376SendCmdWaitInt( CMD0H_DISK_R_SENSE );
	DelayMs( 50 );
	return( s );
}


// ���U���Ƿ�д����, ����USB_INT_SUCCESS˵������д,����0xFF˵��ֻ��/д����,��������ֵ˵���Ǵ������ 
uint8	IsDiskWriteProtect( void )
{
	uint8	s, SysBaseInfo;
	P_BULK_ONLY_CBW	pCbw;

	SysBaseInfo = CH376ReadVar8( VAR_SYS_BASE_INFO );  // ��ǰϵͳ�Ļ�����Ϣ 
	pCbw = (P_BULK_ONLY_CBW)DataBuff;
	for ( s = 0; s != sizeof( pCbw -> CBW_CB_Buf ); s ++ )
		pCbw -> CBW_CB_Buf[ s ] = 0;  // Ĭ����0 
	pCbw -> CBW_DataLen0 = 0x10;  // ���ݴ��䳤�� 
	pCbw -> CBW_Flag = 0x80;  // ���䷽��Ϊ���� 
	if ( SysBaseInfo & 0x40 )
	{  
		// SubClass-Code������6 
		pCbw -> CBW_CB_Len = 10;  // �����ĳ��� 
		pCbw -> CBW_CB_Buf[0] = 0x5A;  // ��������ֽ�, MODE SENSE(10) 
		pCbw -> CBW_CB_Buf[2] = 0x3F;
		pCbw -> CBW_CB_Buf[8] = 0x10;
	}
	else
	{ 
		// SubClass-Code�����Ϊ6 
		pCbw -> CBW_CB_Len = 6;  // �����ĳ��� 
		pCbw -> CBW_CB_Buf[0] = 0x1A;  // ��������ֽ�, MODE SENSE(6) 
		pCbw -> CBW_CB_Buf[2] = 0x3F;
		pCbw -> CBW_CB_Buf[4] = 0x10;
	}

	// ��USB�����˵�ķ��ͻ�����д�����ݿ�,ʣ�ಿ��CH376�Զ�� 
	CH376WriteHostBlock( (uint8 *)pCbw, sizeof( BULK_ONLY_CBW ) );
	// ��U��ִ��BulkOnly����Э�� 
	s = CH376SendCmdWaitInt( CMD0H_DISK_BOC_CMD );  
	if ( s == USB_INT_SUCCESS )
	{
		// �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�,���س��� 
		s = CH376ReadBlock( DataBuff );  
		if ( s > 3 )
		{
			// MODE SENSE��������ݵĳ�����Ч 
			if ( SysBaseInfo & 0x40 )
				s = DataBuff[3];  // MODE SENSE(10), device specific parameter 
			else
				s = DataBuff[2];  // MODE SENSE(6), device specific parameter 
			if ( s & 0x80 )
				return( 0xFF );  // U��д���� 
			else
				return( USB_INT_SUCCESS );  // U��û��д���� 
		}
		return( USB_INT_DISK_ERR );
	}

	CH376DiskReqSense( );  // ���USB�洢������ 
	return( s );
}



// ��ѯ����ʣ��ռ���Ϣ,������ 
uint8	CH376DiskQuery( uint32 *DiskFre )  
{
	uint8	s,temp;
	uint8	c0, c1, c2, c3;

	s = CH376SendCmdWaitInt( CMD0H_DISK_QUERY );
	if ( s == USB_INT_SUCCESS )
	{
		// �ο�CH376INC.H�ļ���CH376_CMD_DATA�ṹ��DiskQuery 
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		temp = xReadCH376Data( );  // ��������sizeof(CH376_CMD_DATA.DiskQuery) 
		temp = xReadCH376Data( );  // CH376_CMD_DATA.DiskQuery.mTotalSector 
		temp = xReadCH376Data( );
		temp = xReadCH376Data( );
		temp = xReadCH376Data( );

		c0 = xReadCH376Data( );  // CH376_CMD_DATA.DiskQuery.mFreeSector 
		c1 = xReadCH376Data( );
		c2 = xReadCH376Data( );
		c3 = xReadCH376Data( );
		*DiskFre = c0 | (uint16)c1 << 8 | (uint32)c2 << 16 | (uint32)c3 << 24;

		temp = xReadCH376Data( );  // CH376_CMD_DATA.DiskQuery.mDiskFat 
		xEndCH376Cmd( );
	}
	else
		*DiskFre = 0;
	return( s );
}



// ���ý�Ҫ�������ļ����ļ��� 
void	CH376SetFileName( uint8 *name )
{
	uint8	c;
	
	xWriteCH376Cmd( CMD10_SET_FILE_NAME );

	c = *name;
	xWriteCH376Data( c );
	while ( c )
	{
		DelayMs(5);
		name ++;
		c = *name;
		if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  // ǿ�н��ļ�����ֹ 
		xWriteCH376Data( c );
	}
	xEndCH376Cmd( );
}


// �ڸ�Ŀ¼���ߵ�ǰĿ¼���½��ļ�,����ļ��Ѿ�������ô��ɾ�� 
uint8	CH376FileCreate( uint8 *name )
{
	// ���ý�Ҫ�������ļ����ļ��� 
	if ( name )
		CH376SetFileName( name );  

	return( CH376SendCmdWaitInt( CMD0H_FILE_CREATE ) );
}


// ��CH376оƬ��ȡ32λ�����ݲ���������
uint32	CH376Read32bitDat( void )  
{
	uint8	c0, c1, c2, c3;
	uint32	res = 0;

	c0 = xReadCH376Data( );
	c1 = xReadCH376Data( );
	c2 = xReadCH376Data( );
	c3 = xReadCH376Data( );
	xEndCH376Cmd( );

	res = c0 | (uint16)c1 << 8 | (uint32)c2 << 16 | (uint32)c3 << 24 ;
	return( res );
}



// ���ڲ�ָ��������д����������ݿ�,���س��� 
uint8	CH376WriteReqBlock( uint8 *buf )
{
	uint8	s, l;

	xWriteCH376Cmd( CMD01_WR_REQ_DATA );
	s = l = xReadCH376Data( );  // ���� 
	if ( l )
	{
		do
		{
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- l );
	}
	xEndCH376Cmd( );
	return( s );
}


//���ֽ�Ϊ��λ��ǰλ��д�����ݿ� 
uint8	CH376ByteWrite( uint8 *buf, uint16 ReqCount, uint16 *RealCount )
{
	uint8	s;

	xWriteCH376Cmd( CMD2H_BYTE_WRITE );
	xWriteCH376Data( (uint8)ReqCount );
	xWriteCH376Data( (uint8)(ReqCount>>8) );
	xEndCH376Cmd( );

	if ( RealCount )
		*RealCount = 0;

	while ( 1 )
	{
		s = Wait376Interrupt( );
		if ( s == USB_INT_DISK_WRITE )
		{
			//���ڲ�ָ��������д����������ݿ�,���س��� 
			s = CH376WriteReqBlock( buf );
			xWriteCH376Cmd( CMD0H_BYTE_WR_GO );
			xEndCH376Cmd( );
			buf += s;
			if ( RealCount )
				*RealCount += s;
		}
		else
			return( s );  // ����
	}
}




// ���������еĶ�����������ݿ�д��U��,��֧��SD�� 
// baStart ��д�������ʼ��������, iSectorCount ��д��������� 
uint8	CH376DiskWriteSec( uint8 *buf, uint32 iLbaStart, uint8 iSectorCount )  
{
	uint8	s, err;
	uint16	mBlockCount;

	for ( err = 0; err != 3; ++ err )
	{
		// �������� 
		xWriteCH376Cmd( CMD5H_DISK_WRITE );  // ��USB�洢��д���� 
		xWriteCH376Data( (uint8)iLbaStart );  // LBA�����8λ 
		xWriteCH376Data( (uint8)( (uint16)iLbaStart >> 8 ) );
		xWriteCH376Data( (uint8)( iLbaStart >> 16 ) );
		xWriteCH376Data( (uint8)( iLbaStart >> 24 ) );  // LBA�����8λ 
		xWriteCH376Data( iSectorCount );  // ������ 
		xEndCH376Cmd( );

		mBlockCount = iSectorCount * DEF_SECTOR_SIZE / CH376_DAT_BLOCK_LEN;
		for ( ; mBlockCount != 0; -- mBlockCount )
		{ 
			// ���ݿ���� 
			s = Wait376Interrupt( );  // �ȴ��жϲ���ȡ״̬ ��Ӧ����0x1E
			if ( s == USB_INT_DISK_WRITE )
			{
				// USB�洢��д���ݿ�,��������д�� 
				CH376WriteHostBlock( buf, CH376_DAT_BLOCK_LEN );  // ��USB�����˵�ķ��ͻ�����д�����ݿ� 
				xWriteCH376Cmd( CMD0H_DISK_WR_GO );  // ����ִ��USB�洢����д���� 
				xEndCH376Cmd( );
				buf += CH376_DAT_BLOCK_LEN;
			}
			else
				break;  // ���ش���״̬ 
		}
		if ( mBlockCount == 0 )
		{
			s = Wait376Interrupt( );  // �ȴ��жϲ���ȡ״̬ 
			if ( s == USB_INT_SUCCESS )
				return( USB_INT_SUCCESS );  // �����ɹ� 
		}
		if ( s == USB_INT_DISCONNECT )
			return( s );  // U�̱��Ƴ� 
		CH376DiskReqSense( );  // ���USB�洢������ 
	}
	return( s );  // ����ʧ�� 
}



// ������Ϊ��λ�ڵ�ǰλ��д�����ݿ�,��֧��SD�� 
//���ݻ�������Ҫд�������������������д������ݸ���
uint8	CH376SecWrite( uint8 *buf, uint8 ReqCount, uint8 *RealCount )  
{
	uint8	s;
	uint8	cnt,temp;
	uint32	StaSec;

	if ( RealCount )
		*RealCount = 0;

	do
	{
		xWriteCH376Cmd( CMD1H_SEC_WRITE );
		xWriteCH376Data( ReqCount );
		xEndCH376Cmd( );

		s = Wait376Interrupt( );
		if ( s != USB_INT_SUCCESS )
			return( s );
		xWriteCH376Cmd( CMD01_RD_USB_DATA0 );
		temp = xReadCH376Data( );  // ��������sizeof(CH376_CMD_DATA.SectorWrite) 

		cnt = xReadCH376Data( );  // CH376_CMD_DATA.SectorWrite.mSectorCount 
		temp = xReadCH376Data( );
		temp = xReadCH376Data( );
		temp = xReadCH376Data( );

		// CH376_CMD_DATA.SectorWrite.mStartSector,��CH376оƬ��ȡ32λ�����ݲ��������� 
		StaSec = CH376Read32bitDat( ); 
		if ( cnt == 0 )
			break;
		s = CH376DiskWriteSec( buf, StaSec, cnt );  // ���������еĶ�����������ݿ�д��U�� 
		if ( s != USB_INT_SUCCESS )
			return( s );
		buf += cnt * DEF_SECTOR_SIZE;
		if ( RealCount ) *RealCount += cnt;
		ReqCount -= cnt;
	} while ( ReqCount );

	return( s );
}


// �����������һ�ֽ����ݺ�,�ȴ��ж� 
uint8	CH376SendCmdDatWaitInt( uint8 mCmd, uint8 mDat )  
{
	xWriteCH376Cmd( mCmd );
	xWriteCH376Data( mDat );
	xEndCH376Cmd( );
	return( Wait376Interrupt( ) );
}



// �رյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼(�ļ���)
uint8	CH376FileClose( uint8 UpdateSz )
{
	return( CH376SendCmdDatWaitInt( CMD1H_FILE_CLOSE, UpdateSz ) );
}