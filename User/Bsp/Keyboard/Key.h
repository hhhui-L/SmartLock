#ifndef __BSP__KEY_H
#define __BSP__KEY_H

#include <stm32f10x.h>

#define uint unsigned int 
#define uchar unsigned char

void Key_Config(void);
int KeyScan(void);
int GetKeyVal(void);

#endif

