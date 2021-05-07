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
#include "Handle_code.h"
#include "Handle_flash.h"
#include "bsp_spi_flash.h"
#include "ff.h"		


extern unsigned char flash_password[];//认证密码
extern int j;
extern FIL fp;// 文件系统句柄
extern FRESULT res;
extern UINT br;//多少字节被正常读出
extern unsigned char password[];



//线程控制块
rt_thread_t FRopen_thread=RT_NULL;//指纹解锁线程控制块
rt_thread_t codeOpen_thread=RT_NULL;//密码解锁线程控制块

extern u16 num;

//密码解锁线程
void codeOpen_entry(void *parameter)
{
		while(1)
		{
		
				num=GetKeyVal();
				if(num!=100)
				{
					
					if(num==13)//按一次13可以解锁，按两次13修改密码
					{
							handle_sys();	
					}	
				}
		}
	
}
//指纹解锁线程
void FRopen_entry(void *parameter)
{
	Handle_AS608();//初始化指纹模块；刷、录、删、验证指纹
	Handle_flash();//初始化文件系统；读取系统密码、认证密码、临时密码
		while(1)
		{
			num=GetKeyVal();
			if(num!=100)
			{
	//			rt_kprintf("num=%d\n\n",num);
				
					if(num==12)
					{
							rt_kprintf("请先输入认证密码:\n\n");
							del_str(password);
							GET_str(password);

								if(j==1)
								{
								res = f_open(&fp,"1:认证密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						

								if(res == FR_OK)
								{
									f_lseek(&fp,0);//配置光标到开头
									res = f_read (&fp,flash_password,f_size(&fp),&br);
									if(res == FR_OK)
										rt_kprintf ("认证密码：%s br= %d\n\n",flash_password,br);	
								}		
								f_close(&fp);//操作完成，一定要关闭文件
								}
								
								if(strcmp((const char *)password,(const char *)flash_password)==0)
								{
									del_str(password);
									Del_FR();//删指纹	
								}else
								{
								rt_kprintf ("认证密码输入错误！！！\n\n");
								}
								
						}
					
					
					if(num==11)//按一次11可以录取指纹，按两次11可修改认证密码
					{
							handle_FR();			
					}
			}
			
			if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
			{
				press_FR();//刷指纹解锁			
			}		
		}
}

