/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 线程管理
  *********************************************************************

  */ 

/*
*************************************************************************
*                             包含的头文件
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
*                             函数声明
*************************************************************************
*/
static void key_thread_entry(void* parameter);


/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
		
//  rt_kprintf("\n\n基于安卓手机的智能门锁设计\n\n");
	
		
		webmsg_sendmq = rt_mq_create("wmsgsend",/* 消息队列名字 */
                     1024,     /* 消息的最大长度 */
                     10,    /* 消息队列的最大容量 */
                     RT_IPC_FLAG_FIFO);/* 队列模式 FIFO(0x00)*/
		  if (webmsg_sendmq!= RT_NULL)
				{
					rt_kprintf("\r\nwmsgsend 消息队列创建成功！\r\n");
				}else
				{
					rt_kprintf("\r\nwmsgsend 消息队列创建失败！\r\n");
				}
		
  
		webmsg_recemb = rt_mb_create("wmsgrec",
													5, 
													RT_IPC_FLAG_FIFO);
		 if (webmsg_recemb != RT_NULL)
		 {
			 rt_kprintf("\r\nwmsgrec 消息邮箱创建成功!\n\n");
		 }else
		 {
			 rt_kprintf("\r\nwmsgrec 消息邮箱创建失败!\n\n");
		 }
     
		
//网络连接和发送消息线程	
		webmsgsend_th =                          /* 线程控制块指针 */
    rt_thread_create( "wmsgst",              /* 线程名字 */
                      newwebconnect_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      3072,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (webmsgsend_th != RT_NULL)
	 {
     rt_thread_startup(webmsgsend_th);
		 rt_kprintf("\r\nwmsgst 线程创建成功!\n\n");
	 }else rt_kprintf("\r\nwmsgst 线程创建失败!\n\n");
	 
	 
//网络消息接收和处理线程 
	 webmsgrece_th =                          /* 线程控制块指针 */
    rt_thread_create( "wmsgrt",              /* 线程名字 */
                      newwebhandle_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      3072,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (webmsgrece_th != RT_NULL)
	 {
     rt_thread_startup(webmsgrece_th);
		 rt_kprintf("\r\nwmsgrt 线程创建成功!\n\n");
	 }else rt_kprintf("\r\nwmsgrt 线程创建失败!\n\n");
	 


//指纹解锁线程
		FRopen_thread =                          /* 线程控制块指针 */
    rt_thread_create( "FRopen_thread",              /* 线程名字 */
                      FRopen_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      8,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (FRopen_thread != RT_NULL)
	 {
		 rt_kprintf("\r\nFRopen_thread 线程创建成功\r\n");
		 rt_thread_startup(FRopen_thread);
	 }
    else
     rt_kprintf("\r\nFRopen_thread 线程创建失败\r\n");
		
	 
//密码解锁线程
		codeOpen_thread =                          /* 线程控制块指针 */
    rt_thread_create( "handle",              /* 线程名字 */
                      codeOpen_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      512,                 /* 线程栈大小 */
                      8,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (codeOpen_thread != RT_NULL)
	 {
		 rt_kprintf("\r\ncodeOpen_thread 线程创建成功\r\n");
		 rt_thread_startup(codeOpen_thread);
	 }
    else
     rt_kprintf("\r\ncodeOpen_thread 线程创建失败\r\n");
		
	}




/********************************END OF FILE****************************/
