#ifndef __G4_H
#define	__G4_H

#include "stm32f10x.h"
#include <stdio.h>
#include <stdbool.h>

// 串口4-UART4
#define  G4_USARTx                   USART3
#define  G4_USART_CLK                RCC_APB1Periph_USART3
#define  G4_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
 #define  G4_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  G4_USART_GPIO_CLK           (RCC_APB2Periph_GPIOC)
#define  G4_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  G4_USART_TX_GPIO_PORT       GPIOB  
#define  G4_USART_TX_GPIO_PIN        GPIO_Pin_10
#define  G4_USART_RX_GPIO_PORT       GPIOB
#define  G4_USART_RX_GPIO_PIN        GPIO_Pin_11

#define  G4_USART_IRQ                USART3_IRQn
#define  G4_USART_IRQHandler         USART3_IRQHandler


//检测引脚
#define  G4_RST_PORT       GPIOB  
#define  G4_RST_PIN        GPIO_Pin_8

#define  G4_LIKA_PORT       GPIOB 
#define  G4_LIKA_PIN        GPIO_Pin_1

#define  G4_STAT_PORT       GPIOB 
#define  G4_STAT_PIN        GPIO_Pin_9

#define	 G4_LIKA_EXIT_PORTSOURCE		GPIO_PortSourceGPIOB
#define  G4_LIKA_EXIT_PINSOURCE     GPIO_PinSource1
#define	 G4_LIKA_EXTI_LINE					EXTI_Line1
#define  G4_LIKA_EXTI_IRQ          	EXTI1_IRQn
#define  G4_LIKA_IRQHandler         EXTI1_IRQHandler




struct G4_UART_BUFF
{
	int16_t num;
	char data[1500]; 
};


#define RX_BUF_MAX_LEN     256                                     //最大接收缓存字节数

extern struct  G4_USARTx_Fram                                  //串口数据帧的处理结构体
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	uint16_t FramLength;
	bool FramFinishFlag; 
} G4_Fram_Record;

void G4_StatePinConfig(void);
void G4_SendArray(uint8_t *array, uint16_t num);
void G4_SendString( char *str);
void G4_USART_Config(void);
bool G4_EnterATMode(void);
bool G4_ExitATMode(void);
bool G4_AT_RESTORE(void);
bool G4_AT_REBT(void);
bool G4_AT_UART_Query(uint32_t *baudrate,uint8_t *parity);
bool G4_AT_UART_Set(uint32_t baudrate,char *parity);
bool G4_AT_LINKSTA(uint8_t *sta);
bool G4_AT_SOCK(char *protocol,char *ip,char *port);
bool G4_AT_HEARTMOD(char *mode);
bool G4_AT_HEARTINFO(char *data);
bool G4_AT_HEARTM(uint32_t time);
bool G4_AT_UARTCLR(uint8_t flag);

bool G4_CheckSTAT(void);
bool G4_CheckLIKA(void);
void G4_RST(void);
void G4_SetLIKAPin(uint8_t mode);

#endif

