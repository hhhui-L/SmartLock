/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �̹߳���
  *********************************************************************

  */ 

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"
#include "key.h"   
#include "usart2.h"
#include "AS608.h"
#include "Handle_as608.h"
#include "string.h"
#include "Handle_code.h"
#include "Handle_flash.h"
#include "open.h"
#include "G4.h"
#include "newweb.h"


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void key_thread_entry(void* parameter);


/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
		
//  rt_kprintf("\n\n���ڰ�׿�ֻ��������������\n\n");
	
		
		webmsg_sendmq = rt_mq_create("wmsgsend",/* ��Ϣ�������� */
                     1024,     /* ��Ϣ����󳤶� */
                     10,    /* ��Ϣ���е�������� */
                     RT_IPC_FLAG_FIFO);/* ����ģʽ FIFO(0x00)*/
		  if (webmsg_sendmq!= RT_NULL)
				{
					rt_kprintf("\r\nwmsgsend ��Ϣ���д����ɹ���\r\n");
				}else
				{
					rt_kprintf("\r\nwmsgsend ��Ϣ���д���ʧ�ܣ�\r\n");
				}
		
  
		webmsg_recemb = rt_mb_create("wmsgrec",
													5, 
													RT_IPC_FLAG_FIFO);
		 if (webmsg_recemb != RT_NULL)
		 {
			 rt_kprintf("\r\nwmsgrec ��Ϣ���䴴���ɹ�!\n\n");
		 }else
		 {
			 rt_kprintf("\r\nwmsgrec ��Ϣ���䴴��ʧ��!\n\n");
		 }
     
		
//�������Ӻͷ�����Ϣ�߳�	
		webmsgsend_th =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "wmsgst",              /* �߳����� */
                      newwebconnect_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      3072,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (webmsgsend_th != RT_NULL)
	 {
     rt_thread_startup(webmsgsend_th);
		 rt_kprintf("\r\nwmsgst �̴߳����ɹ�!\n\n");
	 }else rt_kprintf("\r\nwmsgst �̴߳���ʧ��!\n\n");
	 
	 
//������Ϣ���պʹ����߳� 
	 webmsgrece_th =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "wmsgrt",              /* �߳����� */
                      newwebhandle_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      3072,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (webmsgrece_th != RT_NULL)
	 {
     rt_thread_startup(webmsgrece_th);
		 rt_kprintf("\r\nwmsgrt �̴߳����ɹ�!\n\n");
	 }else rt_kprintf("\r\nwmsgrt �̴߳���ʧ��!\n\n");
	 


//ָ�ƽ����߳�
		FRopen_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "FRopen_thread",              /* �߳����� */
                      FRopen_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      8,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (FRopen_thread != RT_NULL)
	 {
		 rt_kprintf("\r\nFRopen_thread �̴߳����ɹ�\r\n");
		 rt_thread_startup(FRopen_thread);
	 }
    else
     rt_kprintf("\r\nFRopen_thread �̴߳���ʧ��\r\n");
		
	 
//��������߳�
		codeOpen_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "handle",              /* �߳����� */
                      codeOpen_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      8,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (codeOpen_thread != RT_NULL)
	 {
		 rt_kprintf("\r\ncodeOpen_thread �̴߳����ɹ�\r\n");
		 rt_thread_startup(codeOpen_thread);
	 }
    else
     rt_kprintf("\r\ncodeOpen_thread �̴߳���ʧ��\r\n");
		
	}




/********************************END OF FILE****************************/
