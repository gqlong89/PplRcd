#ifndef __HARDWARECONFIG_H__
#define __HARDWARECONFIG_H__
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#define	DEBUG_COM			2
#if DEBUG_COM == 1
#define DEBUG_USART	USART1
#elif DEBUG_COM == 2
#define DEBUG_USART	USART2
#elif DEBUG_COM == 3
#define DEBUG_USART	USART3
#endif

//�����������������
#define RCC_UART1_TX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART1TX                               GPIOB    					 
#define GPIO_PIN_UART1TX                                GPIO_Pin_6					 

#define RCC_UART1_RX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART1RX                               GPIOB    					 
#define GPIO_PIN_UART1RX                                GPIO_Pin_7	

#define RCC_UART2_TX                                    RCC_APB2Periph_GPIOA		 
#define GPIO_PORT_UART2TX                               GPIOA    					 
#define GPIO_PIN_UART2TX                                GPIO_Pin_2					 

#define RCC_UART2_RX                                    RCC_APB2Periph_GPIOA		 
#define GPIO_PORT_UART2RX                               GPIOA    					 
#define GPIO_PIN_UART2RX                                GPIO_Pin_3

#define RCC_UART3_TX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART3TX                               GPIOB    					 
#define GPIO_PIN_UART3TX                                GPIO_Pin_10					 

#define RCC_UART3_RX                                    RCC_APB2Periph_GPIOB		 
#define GPIO_PORT_UART3RX                               GPIOB    					 
#define GPIO_PIN_UART3RX                                GPIO_Pin_11



/////stm32�������DMA������DMAͨ���Ķ�Ӧ��ϵ�ǹ̶��ģ�����������õġ��ο�STM32�Ĳο��ֲ�
typedef struct 
{
	
	DMA_Channel_TypeDef		*dma_rx_base;
	int32_t					dma_rx_flag;
	int32_t 				dma_rx_irq;
	
	
	DMA_Channel_TypeDef		*dma_tx_base;
	int32_t					dma_tx_flag;
	int32_t 					dma_tx_irq;
	
	
	

}Dma_source;

typedef struct {
	//����������оƬ�ĵڼ������ڣ���0��ʼ
	int						uartNum;
	USART_InitTypeDef		*cfguart;
	Dma_source				*dma;
	
}CfgUart_t;


extern CfgUart_t g_confUart2, g_confUart1;
typedef struct 
{
	GPIO_TypeDef	*Port;
	uint16_t		pin;
}gpio_pins;

extern gpio_pins Dir_485_pin1 , Dir_485_pin2;
//extern gpio_pins pin_DugUartTx;
//extern gpio_pins pin_DugUartRx;

//#define	GPRS_COM			3			///< gprsģ��ʹ�õĴ��ں�	
//#define	SERAIL_485_COM		2
//#define W25Q_SPI			SPI1
//#define ADC_BASE			ADC1

//#if GPRS_COM == 3
//#define GPRS_USART	USART3
//#elif GPRS_COM == 1
//#define GPRS_USART	USART1
//#endif



//#if SERAIL_485_COM == 2
//#define SER485_USART	USART2
//#endif









//extern gpio_pins	Gprs_powerkey;

//extern gpio_pins	W25Q_csPin;
//extern SPI_instance W25Q_Spi ;
//extern gpio_pins 	ADC_pins;
//extern gpio_pins	PinLED_run;
//extern gpio_pins	PinLED_com;


//extern int32_t ADC_chn;

//extern USART_InitTypeDef USART_InitStructure;
//extern USART_InitTypeDef Conf_GprsUsart;
//extern USART_InitTypeDef Conf_S485Usart_default;

//extern Dma_source DMA_gprs_usart;
//extern Dma_source DMA_s485_usart;
//extern Dma_source DMA_adc;


//extern gpio_pins ADC_pins_4051A1;
//extern gpio_pins ADC_pins_4051B1;
//extern gpio_pins ADC_pins_4051C1;
//extern gpio_pins ADC_pins_control0;
//extern gpio_pins ADC_pins_control1;
//extern gpio_pins ADC_pins_control2;


#endif
