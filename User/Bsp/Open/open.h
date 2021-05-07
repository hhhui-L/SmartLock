#ifndef __OPEN_H
#define __OPEN_H

#include <stm32f10x.h>
#include "rtthread.h"
#include "board.h"



extern rt_thread_t codeOpen_thread;
extern rt_thread_t FRopen_thread;

void codeOpen_entry(void *parameter);
void FRopen_entry(void *parameter);

#endif

