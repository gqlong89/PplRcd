#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>

#include "deviceId.h"
#include "dev_cmd.h"

#include "dev_char.h"
#include "dev_uart2/dev_uart2.h"


typedef uint32_t devId_t;


//�豸ID 
// | 31 .. 24 	| 23 .. 16 	|	15 .. 8		| 7 .. 0 	|
// | ����		| �豸����	|	���豸��	| ���豸��	|


#define DEVID_RESERVE_BITS		8
#define DEVID_TYPE_BITS		8
#define DEVID_MAJOR_BITS		8
#define DEVID_MINOR_BITS		8

#define DEVID_UART2  0x00010101


//ʹ�ú��������ú���Ϊ�˱����ĸ�����
devId_t Dev_TYPE( devId_t devid);
devId_t Dev_MAJORID( devId_t devid);
devId_t Dev_MINORID( devId_t devid);



int Dev_open( devId_t devId, void **dev);

#endif
