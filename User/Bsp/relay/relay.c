#include "relay.h"
	    

 
//relay IO初始化
void RELAY_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
	
 RCC_APB2PeriphClockCmd(RELAY_GPIO_RCC_CLK, ENABLE);	 //使能PB,PE端口时钟

 GPIO_InitStructure.GPIO_Pin = RELAY0_PIN;				       //LED0端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(RELAY0_GPIO, &GPIO_InitStructure);					   //根据设定参数初始化LED0
 GPIO_ResetBits(RELAY0_GPIO,RELAY0_PIN);						         //LED0输出高

 GPIO_InitStructure.GPIO_Pin = RELAY1_PIN;				       //LED0端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(RELAY1_GPIO, &GPIO_InitStructure);					   //根据设定参数初始化LED0
 GPIO_ResetBits(RELAY1_GPIO,RELAY1_PIN);	
}
 