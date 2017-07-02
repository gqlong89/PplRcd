#ifndef __INC_drive_gpio_H__
#define __INC_drive_gpio_H__


//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "stdint.h"
#include "stm32f10x_gpio.h"
#include "lw_oopc.h"
#include "dri_cmd.h"
#include "hardwareConfig.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define GITP_RISINGEDGE		1			//������
#define GITP_FAILINGEDGE	2			//������
#define GITP_MAX			2			//
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//�ڴ����ж��д������ݣ�������Ҫ���ٴ���Ĳ���
typedef void (*irqHdl)( void *self, int type, int encode);


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
CLASS( driveGpio)
{
	uint8_t		encode[4];
	void		*p_gpioBase;
	void		*p_cfg;
	irqHdl		func_hdl;
	
	int ( *init)( driveGpio *self, void *p_base, void *cfg);
	int ( *deInit)( driveGpio *self);
	int ( *read)( driveGpio *self,int *p_n_val);
	int ( *write)( driveGpio *self, int n_val);
	int ( *ioctol)( driveGpio *self, int cmd, ...);
	
	void (*setIrqHdl)( driveGpio *self, irqHdl hdl);
	void (*setEncode)( driveGpio *self, int e);
	
	
	
	int ( *test)( driveGpio *self, void *testBuf, int bufSize);
	
	
};




#endif
