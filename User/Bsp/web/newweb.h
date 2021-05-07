#ifndef __NEWWEB_H
#define	__NEWWEB_H

#include "stm32f10x.h"
#include <stdbool.h>
#include "G4.h"
#include "rtthread.h"
#include "board.h"
#include "CRC.h"
#include "myaes.h"
#include "cJSON.h"
#include "base64.h"

extern rt_mq_t webmsg_sendmq;//发送内容的消息队列
extern rt_mailbox_t webmsg_recemb;//接收内容的邮箱
extern uint8_t conn_flag;

extern rt_thread_t webmsgsend_th;   //网络连接和发送消息线程
extern rt_thread_t webmsgrece_th;   //网络消息接收和处理线程

void newwebconnect_entry(void *parameter);
void newwebhandle_entry(void *parameter);

#endif

