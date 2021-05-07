#ifndef __RELAY_H_
#define __RELAY_H_

#include "sys.h"

/***************** LED���������궨��(�͵�ƽ����) *********************/
#define RELAY_ON(x)      x=1    //��LED
#define RELAY_OFF(x)     x=0   //�ر�LED
#define RELAY_TOGGLE(x)  x^=1   //��תLED
/****************************** end *********************************/



/*****************relay����GPIO��ʱ�Ӻ궨�� ***************************/
#define RELAY_GPIO_RCC_CLK  RCC_APB2Periph_GPIOB//LED0 GPIO RCCʱ��
/****************************** end *********************************/


/********************* relay����GPIO�ں궨�� ***************************/
#define RELAY0_GPIO  GPIOB
#define RELAY1_GPIO  GPIOB

#define RELAY0_PIN   GPIO_Pin_14
#define RELAY1_PIN   GPIO_Pin_13

#define RELAY0 PBout(14) // PB14
#define RELAY1 PBout(13) // PB13
  
/****************************** end *********************************/



/************************* �������� *********************************/

void RELAY_Init(void);


/****************************** end *********************************/






		 				    
#endif
