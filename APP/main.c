/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include <stdio.h>
#include <string.h>
#include "stm32f10x_usart.h"

#include "hardwareConfig.h"
#include "mySystemClock.h"
#include "pinmux.h"
#include "power.h"
#include "intrInit.h"
#include "sys_cmd.h"
#include "Modbus_app.h"

#include "basis/macros.h"
#include "basis/sdhError.h"
#include "device.h"
#include "TDD.h"
#include "HMI/HMIFactory.h"
#include "HMI/Component_tips.h"

#include "Usb.h"
#include "usb_hardware_interface.h"

#include "control/CtlKey.h"
#include "control/CtlTimer.h"
#include "utils/time.h"
#include "utils/keyboard.h"

#include "unit_test/unit_test.h"



#include "os/os_depend.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
//系统版本号
//主板本号自己制定
//系统编译时的月份和日作为子版本
short  g_majorVer = 1;		
short  g_childVer;
		
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define LCD_NOKIE		0

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static void ThrdKeyRun (void const *argument);                             // thread function
osThreadId tid_Thread_key;                                          // thread id
osThreadDef (ThrdKeyRun, osPriorityNormal, 1, 0);                   // thread object

static usb_op_t	usb_op;

 	uint16_t	main_ms = 0;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

static int	Main_USB_event(int type);
static void Init_usb_when_idle(void *arg);
void 	Init_LCD(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

/*
 * main: initialize and start the system
 */

//extern cmd_run_t 	cmd_run;


int main (void) {
	Keyboard		*p_kb;
	Controller		*p_control;
	
	Model 			*p_mdl_time;
	CMP_tips 		*p_tips;
	

	uint8_t			old_sys_flag = phn_sys.sys_flag;
	
  // initialize peripherals here

	//BSP的初始化
	OpenPrpClock();
	Pin_init();
	NVIC_Configuration();
	
	PVD_Init();

	
	Init_LCD();
	
	osKernelInitialize (); 
	Init_Cmd_Thread();
	// initialize CMSIS-RTOS
	//各个外设驱动模块初始化
	System_init();
	
	
	//控制器初始化
//#if TDD_ON == 0

	
	Unit_test();

	//界面初始化
	HMI_Init();
	//按键初始化
	p_kb = GetKeyInsance();
	
	//借用一下内存:&phn_sys.lcd_cmd_bytes
	phn_sys.lcd_cmd_bytes = CONF_KEYSCAN_CYCLEMS;
	p_kb->init( p_kb, &phn_sys.lcd_cmd_bytes);
	phn_sys.lcd_cmd_bytes  = 0;

	//创建控制器
	p_control = SUPER_PTR( Get_CtlKey(), Controller);
	p_control->init(p_control, p_kb);
	
	p_control = SUPER_PTR(CtlTimer_new(), Controller);
	if(p_control == NULL) while(1);
	p_control->init(p_control, NULL);
	
//创建线程
	
	tid_Thread_key = osThreadCreate (osThread(ThrdKeyRun), p_kb);
	if (!tid_Thread_key) return(-1);
	
	p_mdl_time = Create_model("time");
	
	
	osKernelStart ();  
	
	
	
	Cmd_Rgt_time_task(Init_usb_when_idle, NULL, 5);		//3秒之后初始化USB。

	MBA_Init();
	
	
	
	
	phn_sys.sys_flag |= SYSFLAG_POWEON;
	p_tips = TIP_Get_Sington();
	
	
//	p_tips->show_ico_tips(1, -1);
	
	while(1)
	{
		osDelay(10);
		
		//在调试中发现main_ms出现过错误值:0x40 这样的话因为500ms和100ms任务就永远的不能执行了
		if(main_ms % 10)
			main_ms = 0;
		
		main_ms += 10;
		
		if(old_sys_flag != phn_sys.sys_flag)
		{
			if(phn_sys.sys_flag & SYSFLAG_POWEROFF)
			{
				p_tips->show_ico_tips(1, -1);
				
			}
			else if(phn_sys.sys_flag & SYSFLAG_EFS_NOTREADY)
			{
				p_tips->show_ico_tips(1, -1);
				
			}
			else if(phn_sys.sys_flag & SYSFLAG_ERR)
			{
				p_tips->show_ico_tips(2, -1);
				
			}
			else
			{
				
				p_tips->clear_ico_tips(1);
				p_tips->clear_ico_tips(2);
			}
			old_sys_flag = phn_sys.sys_flag;
		}
		if((main_ms % 500) == 0)		
		{
			p_mdl_time->run(p_mdl_time);
			g_p_curHmi->hmi_run(g_p_curHmi);
//			main_ms = 0;
		}
		USB_Run();
		
//		phn_sys.lcd_sem_wait_ms = 20;
		if((main_ms % 100) == 0)		
			LCD_Run();
		
		STG_Run();
//		phn_sys.lcd_sem_wait_ms = 0xffffffff;
//		osThreadYield (); 

//		if(cmd_run.func)
//			cmd_run.func(cmd_run.arg);

	}
//#elif TDD_TIME_SEC == 1	//tdd_on == 0
//	TDD_Time_sec();
//#elif TDD_EFS == 1
//	TDD_Efs();
//#elif TDD_FM25 == 1
//	TDD_Fm25();
//#elif TDD_W25Q == 1
//	TDD_W25q();
//#elif TDD_MODCHANNEL == 1
//	TDD_Mdl_chn();
//#elif TDD_DEV_UART3 == 1
//	TDD_Uart_3();
//# elif TDD_SMART_BUS == 1
//	TDD_Smart_bus();
//# elif TDD_USB == 1
//	TDD_Usb();
//#	elif TDD_SHEET == 1
//	TDD_Sheet();
//#	elif TDD_KEYBOARD == 1
//	TDD_Keyboard();
//#	elif TDD_GPIO == 1
//	TDD_Gpio();
//#	elif TDD_MVC == 1
//	TDD_Mvc();
//#	elif TDD_DEV_UART2 == 1	
//	TDD_Uart_2();
//#endif		//TDD_ON == 0
  
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{



static void ThrdKeyRun (void const *argument) {
	Keyboard	*p_kb = ( Keyboard	*)argument ;
	while (1) { 
		delay_ms(CONF_KEYSCAN_CYCLEMS);		
		p_kb->run( p_kb);
		osThreadYield ();                                           // suspend thread
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData( DEBUG_USART, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus( DEBUG_USART, USART_FLAG_TC) == RESET)
    {
    }

    return ch;
}

int fgetc(FILE *f /*stream*/)
{
	 /* Loop until rx not empty */
    while (USART_GetFlagStatus( DEBUG_USART, USART_FLAG_RXNE) == RESET)
    {
    }
	
	return USART_ReceiveData( DEBUG_USART);
}


void HardFault_Handler()
{
	while(1) {
	;
	}
	
}

static void Init_usb_when_idle(void *arg)
{
	
	UHI_Init(&usb_op);
	if(USB_Init(&usb_op) != RET_OK)
	{
		phn_sys.sys_flag |= SYSFLAG_ERR;
	}
	else
	{
		
		USB_Rgt_event_hdl(Main_USB_event);
	}
}

static int	Main_USB_event(int type)
{
	CMP_tips *p_tips = TIP_Get_Sington();
	if(type == et_ready)
	{
		
		phn_sys.usb_device = 1;
		p_tips->show_ico_tips(0, -1);
		
	}
	else if(type == et_remove)
	{
		phn_sys.usb_device = 0;
		p_tips->clear_ico_tips(0);
		
	}
	
	return 0;
}

void 	Init_LCD(void)
{
	I_dev_lcd 				*tdd_lcd;

	Dev_open(LCD_DEVID, (void *)&tdd_lcd);
	tdd_lcd->open();
	tdd_lcd->Clear(COLOUR_BLUE);
//	tdd_lcd->done();
//	Flush_LCD();
//	LCD_Run();	//立即执行lcd指令
}
