
#ifndef __USB_H__
#define __USB_H__

#include "config.h"
//#include "ConfigInfo.h"
//#include "Func.h"
#include "Ch376.h"
#include "Ch376INC.h"



#define YEAR_POSITION     		9 
#define MONTH_POSITION    		5 
#define DATA_POSITION     		0 
#define HOUR_POSITION     		(11 + 16)
#define MINUTE_POSITION  		(5 + 16) 
#define SECOND_POSITION  		(0 + 16)



//USB�豸������Ϣ����
// #define	NOSUPPORTDEV		0	//�豸������
// #define	SUPPORTDEV			1	//�豸�Ѳ���
// #define	FILESYSTEMERR		2	//�ļ�ϵͳ����
// #define	FILECOPYCOMPLETE	3	//�ļ��������
// #define	FILECOPYERR			4	//�ļ���������
// #define	USBWRITEPROTECT		(5)	//U��д����

#define	NOSUPPORTDEV		0	//�豸������
#define	SUPPORTDEV			1	//�豸�Ѳ���
#define U_HAVEREADY			(2)	//U���Ѿ���
#define U_NOHAVEREADY		(3)	//U��δ����
#define U_GETINFO			(4)	//U���ѻ�ȡ��Ʒ��Ϣ
#define U_NOGETINFO			(5)	//U��δ��ȡ��Ʒ��Ϣ
#define U_NOPROTECT			(6)	//U��δд����
#define	U_PROTECT			(7)	//U��д����
#define	U_QUREY				(8)	//U�̻�ȡ���ÿռ�
#define	U_NOQUREY			(9)	//U��δ��ȡ���ÿռ�
#define	U_CREAT				(10)//U���½��ļ��ɹ�
#define	U_NOCREAT			(11)//U���½��ļ�ʧ��
#define	U_WRITE				(12)//U��д�ļ��ɹ�
#define	U_NOWRITE			(13)//U��д�ļ�ʧ��
#define	U_CLOSE				(14)//U�̹ر��ļ��ɹ�
#define	U_NOCLOSE			(15)//U�̹ر��ļ�ʧ��

#define	U_PROTECTERROR		(16)//U��д������ѯ����



#define USB_NOOPER			(0)		//U��δ����
#define USB_GETINFO			(1)		//U�̳�����Ϣ
#define USB_CHECKPROTECT	(2)		//����Ƿ�д����
#define USB_QUREYINFO		(3)		//��ȡU�̿��ÿռ�
#define USB_CREATFILE		(4)		//�����ļ�
#define USB_WRITEFILE		(5)		//д�ļ�
#define USB_CLOSEFILE		(6)		//�ر��ļ�






//������ַ����
#define	SYSPARA_USB_ADDR		1600
#define	OUTPARA_USB_ADDR		(SYSPARA_USB_ADDR+44)
#define	GROUP_USB_ADDR			(OUTPARA_USB_ADDR+16)
#define	CONFIGPARA_USB_ADDR		(GROUP_USB_ADDR+72)	


//����汾����Usb�ļ������еĵ�ַ
#define EDIDION_ADDR	(0x3FF0)


#define SECTORNUM	(32)	//ÿ��д��U�̵���������16*1024/512




extern 	uint8	UsbExist;
extern  uint16	Buff_Free;	//U �̿��ÿռ�
extern	uint8	g_IsDiskWriteProtect;
extern	uint8  UsbReadyFlag;


extern	void	Reset_Ch376(void);
extern	void	InsertUdisk(void);
extern	void	RemoveUdisk(void);

extern	void	DealWithUsb(void);
extern	void DetectUsb(void);


#endif