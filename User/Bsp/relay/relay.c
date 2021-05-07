#include "relay.h"
	    

 
//relay IO��ʼ��
void RELAY_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
	
 RCC_APB2PeriphClockCmd(RELAY_GPIO_RCC_CLK, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

 GPIO_InitStructure.GPIO_Pin = RELAY0_PIN;				       //LED0�˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(RELAY0_GPIO, &GPIO_InitStructure);					   //�����趨������ʼ��LED0
 GPIO_ResetBits(RELAY0_GPIO,RELAY0_PIN);						         //LED0�����

 GPIO_InitStructure.GPIO_Pin = RELAY1_PIN;				       //LED0�˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(RELAY1_GPIO, &GPIO_InitStructure);					   //�����趨������ʼ��LED0
 GPIO_ResetBits(RELAY1_GPIO,RELAY1_PIN);	
}
 