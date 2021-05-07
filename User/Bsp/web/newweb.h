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

extern rt_mq_t webmsg_sendmq;//�������ݵ���Ϣ����
extern rt_mailbox_t webmsg_recemb;//�������ݵ�����
extern uint8_t conn_flag;

extern rt_thread_t webmsgsend_th;   //�������Ӻͷ�����Ϣ�߳�
extern rt_thread_t webmsgrece_th;   //������Ϣ���պʹ����߳�

void newwebconnect_entry(void *parameter);
void newwebhandle_entry(void *parameter);

#endif

