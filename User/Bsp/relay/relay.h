#ifndef __RELAY_H_
#define __RELAY_H_

#include "sys.h"

/***************** LED操作函数宏定义(低电平点亮) *********************/
#define RELAY_ON(x)      x=1    //打开LED
#define RELAY_OFF(x)     x=0   //关闭LED
#define RELAY_TOGGLE(x)  x^=1   //翻转LED
/****************************** end *********************************/



/*****************relay所在GPIO口时钟宏定义 ***************************/
#define RELAY_GPIO_RCC_CLK  RCC_APB2Periph_GPIOB//LED0 GPIO RCC时钟
/****************************** end *********************************/


/********************* relay所在GPIO口宏定义 ***************************/
#define RELAY0_GPIO  GPIOB
#define RELAY1_GPIO  GPIOB

#define RELAY0_PIN   GPIO_Pin_14
#define RELAY1_PIN   GPIO_Pin_13

#define RELAY0 PBout(14) // PB14
#define RELAY1 PBout(13) // PB13
  
/****************************** end *********************************/



/************************* 函数声明 *********************************/

void RELAY_Init(void);


/****************************** end *********************************/






		 				    
#endif
