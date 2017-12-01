//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
//�ṩ��U�̻�SD����SD������û��������ʵ�֣����ļ���CRUD����
//�汾��V010
#include "Usb.h"
#include "Ch376.h"
#include "os/os_depend.h"
#include "deviceId.h"
#include "sdhDef.h"
#include "arithmetic/cycQueue.h"
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

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NUM_EHDS					3
#define LEN_USB_CQBUF			16
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	
	CycQueus_t	usb_cq;
	
	//USB���¼�֪ͨ�ص�����
	usb_event_hdl	arr_event_hdl[3];
	
	uint16_t			buf_free_MB;
	uint8_t			set_ehd;		//bit Ϊ1˵����hdl����
	uint8_t			cur_state;
	uint8_t			err_status;
	char				is_usb_exist;
	char				is_protect;
	uint8_t			none	;
	
	
}usb_control_t;

typedef int	(*usb_deal_msg)(void);
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static 	usb_control_t		usb_ctl;
static	uint8_t 				usb_cq_buf[LEN_USB_CQBUF];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static  void Usb_intr(int ch376_status);

/***** deal msg ***************************/
static int	Usb_deal_insert(void);
static int	Usb_deal_remove(void);
static int	Usb_deal_identify(void);

static usb_deal_msg			arr_deal_msg[USB_MSG_INDEX(usb_msg_max)] = {Usb_deal_insert, Usb_deal_remove, Usb_deal_identify};

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int USB_Run(void* arg)
{
	int			ret = RET_OK;
	uint8_t	msg = 0;
	uint8_t	i = 0;
	CQ_Read(&usb_ctl.usb_cq, &msg, 1);

	if((msg < usb_msg_max) &&(msg > 0))
	{
		arr_deal_msg[USB_MSG_INDEX(msg)]();
		
	} 
	else if((msg > 0))
	{
		//���¼������ⲿ
		for(i = 0; i < NUM_EHDS; i++)
		{
			if(usb_ctl.arr_event_hdl[i])
			{
				
				usb_ctl.arr_event_hdl[i](msg);
			}
			
		}
		
	}
	
	//���������Ե�һЩ����
	
	
	
	return ret;
}


int USB_Init(void* arg)
{
	int	ret = -1;
	//usb��������ʼ��
	usb_ctl.set_ehd = 0xff;
	CQ_Init(&usb_ctl.usb_cq, usb_cq_buf, LEN_USB_CQBUF);
	
	//ch376Ӳ����ʼ��
	
	Power_Ch376(1);
	ret = Init_Ch376(DEVID_SPI1, Usb_intr);
	if( ret == USB_INT_SUCCESS)
		ret = RET_OK;
	return ret;
	
}

int USB_Open_file(char *file_name, char mode)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_Colse_file(int fd)
{
	int	ret = RET_OK;
	
	
	return ret;
}


int USB_Get_file_info(char *file_name, USB_file_info *finfo)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_Get_file_info_f(int fd, USB_file_info *finfo)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_Create_file(char *file_name, char mode)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_Delete_file(char *file_name, char mode)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_Read_file(int fd, char *buf, int len)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_Write_file(int fd, char *buf, int len)
{
	int	ret = RET_OK;
	
	
	return ret;
}

int USB_flush_file(int fd)
{
	int	ret = RET_OK;
	
	
	return ret;
	
}

void	USB_Power_off(void)
{
	
	
}

int	USB_Remove_disk(void )
{
	int	ret = RET_OK;
	
	
	return ret;
}

int	USB_Rgt_event_hdl(usb_event_hdl hdl)
{
	
	int	i = 0;
	
	for(i = 0; i < NUM_EHDS; i++)
	{
		if(usb_ctl.set_ehd & (1 << i))
		{
			usb_ctl.arr_event_hdl[i] = hdl;
			
			usb_ctl.set_ehd &= ~(1 << i);
			break;
		}
		
	}
	if(i == NUM_EHDS)
		i = -1;
	
	return i; 
}


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{


static  void Usb_intr(int ch376_status)
{
	//U�̲����ʱ�򣬻�����жϣ�����SD�������ǲ�������жϵ�
	
	uint8_t		msg = 0;
	switch(ch376_status)
	{
		case USB_INT_CONNECT:
			//��⵽U���������ˣ�������Ӧ�ò�ѯU�̵Ļ�����Ϣ
			msg = hd_storage_insert;
			break;
		case USB_INT_DISCONNECT:
			//��⵽U�̶Ͽ�
			msg = hd_storage_remove;
			
			break;
		
		
	}
	
	if(msg)
		CQ_Write(&usb_ctl.usb_cq, &msg, 1);
	
//	for(i = 0; i < NUM_EHDS; i++)
//	{
//		if(usb_ctl.arr_event_hdl[i])
//		{
//			
//			usb_ctl.arr_event_hdl[i](0);
//		}
//		
//	}
}


//���USB�Ƿ�����γ�
//static void DetectUsb(void)
//{
//	int	g_status;

//	g_status = Ch376DiskConnect();
////	if (UsbExist == 0)
////	{
////		if (g_status == USB_INT_SUCCESS)
////		{
////			InsertUdisk();
////			UsbPara.ErrStatu = SUPPORTDEV;
////			
////		}
////		else
////		{
////			UsbPara.ErrStatu = NOSUPPORTDEV;
////		}
////	}
////	else
////	{
////		if (g_status != USB_INT_SUCCESS)
////		{
////			RemoveUdisk();
////		}
////	}
//}


///*
//**************************************************************************
//* �������ƣ�InsertUdisk
//* �����������
//* �����������
//* ���������������жϵĹ��Ӻ���
//**************************************************************************
//*/
//void InsertUdisk(void)
//{
//	UsbPara.UsbIrq = 1;
//	g_IsDiskWriteProtect = 0;
//	UsbExist = TRUE;

//}



///*
//*************************************************************************
// �������ƣ�RemoveUdisk
// �����������
// �����������
// ����������
//*************************************************************************
//*/
//void RemoveUdisk(void)
//{
//	UsbExist = FALSE;
//	UsbPara.ErrStatu = NOSUPPORTDEV;
//	UsbPara.ProgressNum = 0;
//	Buff_Free = 0;
//	g_IsDiskWriteProtect = 0;
//	g_PressCopy = 0;

//	UsbPara.Step = 0;
//	UsbPara.UsbIrq = 0;
//	UsbPara.UsbStatu = 0;

//	Reset_Ch376();
//}


static int	Usb_deal_insert(void)
{
	int	ret = RET_OK;
	uint8_t		s = 0;
	uint8_t		msg = 0;
	uint16_t		safe_count = 1000;
	uint8_t		usb_info[40] = {0};		//һ��36���ֽ�
	Ch376_enbale_Irq(0);
	while(1)
	{
		s = Ch376DiskConnect();
		//������ص�״̬��USB_INT_SUCCESS��˵��U�̻����豸����
		if(s == USB_INT_SUCCESS)
		{
			
			
			s = CH376DiskMount();
			if(s == USB_INT_SUCCESS)
			{
				usb_ctl.is_usb_exist = 1;
				
 				//��ȡU�̵ĳ��̺Ͳ�Ʒ��Ϣ���������ݸ���
				//��ЩU�̿��ܲ�֧�֣���˾Ͳ���Ϊ�ж�U�̺û��������ˣ���ȡ����ֻ�����ڵ���ʹ��
 				s = CH376ReadBlock(usb_info, 40);

				usb_ctl.cur_state = SUPPORTDEV;
				msg = sf_identify_device;
				CQ_Write(&usb_ctl.usb_cq, &msg, 1);
				break;
			}
			else 
			{
				safe_count --;
				
			}
			
		}
		else 
		{
			
			msg = hd_storage_remove;
			CQ_Write(&usb_ctl.usb_cq, &msg, 1);
			break;
		}
		
		if(safe_count == 0)
		{
			msg = et_unkow_device;
			CQ_Write(&usb_ctl.usb_cq, &msg, 1);
			break;
			
		}
		delay_ms(10);
	}
	Ch376_enbale_Irq(1);
	
	
	return ret;
	
	
}
static int	Usb_deal_remove(void)
{
	int	ret = RET_OK;
	uint8_t	msg = 0;
	
	usb_ctl.cur_state = NOSUPPORTDEV;
	msg = et_remove;
	CQ_Write(&usb_ctl.usb_cq, &msg, 1);
	return ret;	
	
}

static int	Usb_deal_identify(void)
{
	int					ret = RET_OK;
	uint32_t		sectornum = 0;
	
	uint8_t			s;
	uint8_t			msg = 0;
	uint16_t		i;
	
	if(usb_ctl.cur_state == NOSUPPORTDEV)		//�Ѿ����γ�������
		return ret;
	
	Ch376_enbale_Irq(0);
	
	//���U�̵�д����״̬
	//������ܺ���Ҳ��һ����֧��
	for (i=0;i<3;i++)
	{
		s = IsDiskWriteProtect();
		if(s == USB_INT_SUCCESS)
		{

			usb_ctl.is_protect = 0;

			break;
		}
		else if (s == 0xFF)	//˵����д�����ˣ�Ӧ��ʾ�û�
		{
			usb_ctl.is_protect = 1;
			msg = et_protect;
			CQ_Write(&usb_ctl.usb_cq, &msg, 1);
			goto exit;
		}
	}
	if(i == 3)
		usb_ctl.err_status = U_PROTECTERROR;
	
	//��ѯU�̿��ÿռ�
	for (i=0;i<3;i++)
	{

		s = CH376DiskQuery(&sectornum);
		if(s == USB_INT_SUCCESS)
		{
			if (sectornum)
			{
				usb_ctl.buf_free_MB = sectornum * DEF_SECTOR_SIZE / 1024 / 1024;
			}

			return;
		}
	}
	
	if(i == 3)
	{
		usb_ctl.err_status = U_NOQUREY;
		usb_ctl.buf_free_MB = 0;
		
	}
	
	
exit:
	usb_ctl.cur_state = U_HAVEREADY;
	msg = et_ready;
	CQ_Write(&usb_ctl.usb_cq, &msg, 1);	

	Ch376_enbale_Irq(1);
	return ret;	
	
}


//#include "Usb.h"


//uint8	UsbExist;
//uint16	Buff_Free;	//U �̿��ÿռ�(MBytes)
//uint32	Sectornum;	//�ܵĿ��е���������Ϊ�˼���U�̿��ÿռ�
//uint8	g_UsbBuff[16*1024];
//uint8	g_IsDiskWriteProtect = 0;
//uint8  UsbReadyFlag = 0;


///*
//**************************************************************************
//* �������ƣ�UsbCBGetSysTime
//* �����������
//* �����������ǰʱ��
//* ����������ȡ��ϵͳʱ��
//**************************************************************************
//*/
//uint32  UsbCBGetSysTime(void)
//{
//	uint32 year, month, data;
//	uint32 hour, minute, second;
//	uint32 value;
//	
//	/* this is sample codes, users should get actual time from the system */
//	year = SysTime.year;		//��ķ�ΧΪ0~99
//	year+=2000;
//	month = SysTime.month;
//	data =  SysTime.day;
//	hour =  SysTime.hour;
//	minute = SysTime.minute;
//	second =  SysTime.second;
//	
//	value = hour << HOUR_POSITION |minute << MINUTE_POSITION | second <<SECOND_POSITION |
//		(year-1980)<< YEAR_POSITION |month <<MONTH_POSITION |data << DATA_POSITION;
//	return value;
//}




///*
//**************************************************************************
//* �������ƣ�Reset_Ch376
//* �����������
//* �����������
//* ������������λch376оƬ
//**************************************************************************
//*/

////DelayMs(50) : ���ж��뿪�ж�֮����8.4 ms
////DelayMs(100) : ���ж��뿪�ж�֮����16 ms
//void  Reset_Ch376(void)
//{
//	
//	HRst_Ch376();
//	mInitCH376Host();
//}








///*
//*************************************************************************
// �������ƣ�ENTER_CRITICAL
// �����������
// �����������
// �������������ж�
//*************************************************************************
//*/
//void ENTER_CRITICAL(void)
//{
//	unsigned short temp;
//	
//	__asm
//	{
//		mrs	temp,CPSR
//		orr temp,temp,#0x80
//		msr CPSR_c,temp
//	}
//}


///*
//*************************************************************************
// �������ƣ�EXIT_CRITICAL
// �����������
// �����������
// ����������ȫ���ж�ʹ��
//*************************************************************************
//*/
//void EXIT_CRITICAL(void)
//{
//	unsigned short temp;
//	
//	__asm
//	{
//		mrs	temp,CPSR
//		bic temp,temp,#0x80
//		msr CPSR_c,temp
//	}
//}




///*
//*************************************************************************
// �������ƣ�UsbCreatFileHead
// �����������
// �����������
// ��������������USB�ļ���Ϣ�β��ֵ�����
//*************************************************************************
//*/
//void UsbCreatFileHead(void)
//{
//	uint16	i,j;
//	uint8	*pt;
//	Get_Batch_information(UsbPara.StarCh, UsbPara.EndCh, &UsbPara.StatTime, &UsbPara.EndTime);
//	UsbPara.AddTimes=1;
//	for(i=UsbPara.StarCh;i<=UsbPara.EndCh;i++)
//			UsbPara.AddTimes+=BATCH_Achan_sta[i].Batch_blonum;
//	UsbPara.ProgressNum=0;
//	UsbPara.CurTimes=0;
//	UsbPara.StarDptr=0;
//	UsbPara.NowDptr=0;
//	UsbPara.EndDptr=BATCH_Achan_sta[UsbPara.StarCh].Batch_blonum;
//	UsbPara.Length=BATCH_Achan_sta[UsbPara.StarCh].Batch_blonum;
//	UsbPara.NowCh=UsbPara.StarCh;
//	for(i=0;i<0x4000;i++)	g_UsbBuff[i]=0;
//	for(i=0;i<12;i++)
//		g_UsbBuff[i]=UsbPara.FileName[i];	/*�ļ���*/
//	pt=(uint8 *)&SysTime;
//	for(i=0;i<6;i++)
//		g_UsbBuff[12+i]=*pt++;			/*�ļ�����ʱ��*/
//	g_UsbBuff[18]=UsbPara.StarCh;			/*��ʼͨ����*/
//	g_UsbBuff[19]=UsbPara.EndCh;			/*����ͨ����*/
//	pt=(uint8 *)&UsbPara.StatTime;				/*�û��趨����ʼʱ��*/
//	for(i=20;i<26;i++)
//		g_UsbBuff[i]=*pt++;
//	pt=(uint8 *)&UsbPara.EndTime;				/*�û��趨�Ľ���ʱ��*/
//	for(i=26;i<32;i++)
//		g_UsbBuff[i]=*pt++;
//	pt=(uint8 *)Sec_of_channo_para;
//	for(i=UsbPara.StarCh;i<=UsbPara.EndCh;i++)/*����ͨ����ռ�õļ�¼��*/
//	{
//		for(j=0;j<96;j++)
//			g_UsbBuff[32+i*98+j]=pt[i*96+j];/*��ͨ���Ĳ�����Ϣ*/
//		g_UsbBuff[32+i*98+96]=(BATCH_Achan_sta[i].Batch_blonum>>8)&0xff;
//		g_UsbBuff[32+i*98+97]=BATCH_Achan_sta[i].Batch_blonum&0xff;
//	}
//	pt=(uint8 *)&SysPara;
//	for(i=SYSPARA_USB_ADDR;i<(SYSPARA_USB_ADDR+44);i++)
//		g_UsbBuff[i]=*pt++;				/*ϵͳ����*/
//	pt=(uint8 *)&OutConfig;
//	for(i=OUTPARA_USB_ADDR;i<(OUTPARA_USB_ADDR+16);i++)
//		g_UsbBuff[i]=*pt++;				/*������̬����*/
//	pt=(uint8 *)&CurveGroup[0];
//	for(i=GROUP_USB_ADDR;i<(GROUP_USB_ADDR+72);i++)
//		g_UsbBuff[i]=*pt++;				/*������Ϣ*/
//	pt=(uint8 *)&ConfigPara;			
//	for(i=CONFIGPARA_USB_ADDR;i<(CONFIGPARA_USB_ADDR+SIZE_OF_CONFIGURATION);i++)
//		g_UsbBuff[i]=*pt++;				/*����̬����*/

//	//����汾��V2.1
//	g_UsbBuff[EDIDION_ADDR] = 22;
//}



//void DealWithUsb(void)
//{
//	int	g_status;
//	int	i;
//	uint8	FileName[] = "\\DAT01001.DAT\\";


//	if(UsbPara.UsbIrq && (UsbPara.InitWait>30))
//	{
//		UsbPara.UsbIrq = 0;
//		UsbPara.InitWait = 0;
//		for (i=0;i<3;i++)
//		{
//			DelayMs(100);
//			g_status = CH376DiskMount();	//��ʼ��U�̲����U���Ƿ����
//			if(g_status == USB_INT_SUCCESS)
//			{
//				UsbPara.Step=USB_CREATFILE;
//				UsbPara.ErrStatu = U_HAVEREADY;
//				return;
//			}
//			else if (g_status == ERR_DISK_DISCON)
//			{
//				//��⵽U�̶Ͽ������¼��
//				UsbPara.UsbIrq = 1;
//				UsbPara.InitWait = 30;
//				return;
//			}
//			OpenWdt();
//// 			g_status = Ch376GetDiskStatus();
//// 			if ((g_status >= DEF_DISK_MOUNTED) && (i >= 5))
//// 			{
//// 				UsbPara.Step=USB_CREATFILE;
//// 				UsbPara.ErrStatu = U_HAVEREADY;
//// 				return;
//// 			}
//			
//		}
//		UsbPara.ErrStatu = U_NOHAVEREADY;
//		UsbPara.Step=USB_NOOPER;
//	}

//	//1��2��3������ʱδ�������ǵ���ЩU�̲�֧��
//	//4��5��6����ֻ���ڰ��¸��Ƽ��Ժ����Ч
//	if(UsbPara.Step>0)
//	{
//		
//		switch(UsbPara.Step)
//		{
//// 			case 1:
//// 				//��ȡU�̵ĳ��̺Ͳ�Ʒ��Ϣ���������ݸ���
//// 				g_status = CH376ReadBlock(Buff);
//// 				if(g_status)
//// 				{
//// 					UsbExist=TRUE;
//// 					UsbPara.UsbStatu=FALSE;
//// 					UsbPara.Step=USB_CHECKPROTECT;
//// 					UsbPara.ErrStatu = U_GETINFO;
//// 				}
//// 				else
//// 				{
//// 					UsbPara.ErrStatu = U_NOGETINFO;
//// 					UsbPara.Step=USB_NOOPER;
//// 				}
//// 				break;
//// 
//// 			case 2:
//// 				//���U���Ƿ�д����
//// 				for (i=0;i<3;i++)
//// 				{
//// 					g_status = IsDiskWriteProtect();
//// 					if(g_status == USB_INT_SUCCESS)
//// 					{
//// 						UsbPara.Step=USB_QUREYINFO;
//// 						UsbPara.fInOut=1;
//// 						UsbPara.ErrStatu = U_NOPROTECT;
//// 						return;
//// 					}
//// 					else if (g_status == 0xFF)	//˵����д�����ˣ�Ӧ��ʾ�û�
//// 					{
//// 						g_IsDiskWriteProtect = 1;
//// 						UsbPara.ErrStatu = U_PROTECT;
//// 						return;
//// 					}
//// 				}
//// 				UsbPara.ErrStatu = U_PROTECTERROR;
//// 				break;
//// 			
//// 			case 3:
//// 				//��ѯU�̿��ÿռ�
//// 				for (i=0;i<3;i++)
//// 				{
//// 
//// 					g_status = CH376DiskQuery(&Sectornum);
//// 					if(g_status == USB_INT_SUCCESS)
//// 					{
//// 						if (Sectornum)
//// 						{
//// 							Buff_Free = Sectornum * DEF_SECTOR_SIZE / 1024 / 1024;
//// 						}
//// 						UsbPara.Step=USB_WRITEFILE;
//// 						UsbPara.fInOut=1;
//// 						UsbPara.ErrStatu = U_QUREY;
//// 						return;
//// 					}
//// 				}
//// 				UsbPara.ErrStatu = U_NOQUREY;
//// 				break;

//			
//			case	4:
//				if(g_PressCopy==1)
//				{
//					// �ڸ�Ŀ¼���½��ļ�����,����ļ��Ѿ���������ɾ�������½� 
//					UsbCreatFileHead();
//					memcpy(FileName+1,UsbPara.FileName,12);
//					g_status = CH376FileCreate(FileName);
//					if(g_status == USB_INT_SUCCESS)
//					{
//						UsbPara.Step = USB_WRITEFILE;
//						UsbPara.ErrStatu = U_CREAT;
//					}
//					else
//					{
//						UsbPara.ErrStatu = U_NOCREAT;//�ļ���д����
//						UsbPara.UsbStatu=0;
//						UsbPara.Step=USB_NOOPER;
//					}
//				}
//				break;
//			
//			
//			case	5:
//				//д�ļ�����
//				//if(UsbPara.UsbStatu==TRUE)
//				{
//					if(UsbPara.FristFlag == 1 )
//					{
//						g_status = CH376SecWrite( g_UsbBuff, SECTORNUM, NULL ); 
//						if (g_status != USB_INT_SUCCESS)  			
//						{
//							UsbPara.ErrStatu = U_NOWRITE;//�ļ�д����
//							UsbPara.UsbStatu=0;
//							UsbPara.Step=USB_NOOPER;
//							return;
//						}
//						UsbPara.FristFlag=0;
//						UsbPara.UsbStatu = TRUE;
//					}
//					else
//					{
//						if(UsbPara.UsbStatu==TRUE)
//						{
//							Read_Data_Batch(UsbPara.NowCh, UsbPara.NowDptr++, g_UsbBuff);
//							if(UsbPara.NowDptr >= UsbPara.EndDptr)
//							{
//								UsbPara.NowCh ++;
//								if(UsbPara.NowCh >UsbPara.EndCh)
//								{
//									UsbPara.Step = USB_CLOSEFILE;//�ļ�д����
//								}
//								else
//								{
//									UsbPara.StarDptr = 0;
//									UsbPara.NowDptr = 0;
//									UsbPara.EndDptr = BATCH_Achan_sta[UsbPara.NowCh].Batch_blonum;
//								}
//							}
//							g_status = CH376SecWrite( g_UsbBuff, SECTORNUM, NULL ); 
//							if (g_status != USB_INT_SUCCESS)  			
//							{ 
//								UsbPara.ErrStatu = U_NOWRITE;//�ļ���д����
//								UsbPara.UsbStatu=0;
//								UsbPara.Step=USB_NOOPER;
//								return;
//							}
//						}
//					}

//					if(UsbPara.AddTimes > UsbPara.CurTimes)
//					{
//						UsbPara.CurTimes ++;
//						UsbPara.ProgressNum = UsbPara.CurTimes*200/UsbPara.AddTimes;
//					}

//				}
//			
//				break;
//				
//			
//			case	USB_CLOSEFILE:
//				//�ر��ļ�
//				g_status = CH376FileClose( TRUE );  // �ر��ļ� 
//				if(g_status == USB_INT_SUCCESS)
//				{
//					UsbPara.ErrStatu = U_CLOSE;
//					UsbPara.Step = USB_NOOPER;
//				}
//				else
//				{
//					UsbPara.ErrStatu = U_NOCLOSE;
//					UsbPara.Step=USB_NOOPER;
//				}
//				UsbPara.UsbStatu=0;
//				break;


//			default:
//				break;
//		}
//	}
//}


