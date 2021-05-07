#ifndef __HANDLE_FLASH_H
#define	__HANDLE_FLASH_H


#include "stm32f10x.h"
#include <stdio.h>

void Handle_flash(void);

void write_f(const char *WRITE);
char *read_f(const char *READ);
#endif

