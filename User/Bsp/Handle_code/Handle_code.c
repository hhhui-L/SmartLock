#include "board.h"
#include "rtthread.h"
#include "key.h"
#include "Handle_code.h"
#include "Handle_as608.h"
#include "string.h"
#include "AS608.h"
#include "bsp_spi_flash.h"
#include "ff.h"		
#include "Handle_flash.h"
#include "newweb.h"
#include "beep.h"
#include "relay.h"

#define MAX 100
extern u16 num;
u16 code;
int sign=0;
unsigned char password[MAX];//指纹录入密码
unsigned char openinig_password[MAX];//开锁密码
unsigned char flash_password[MAX]="12345678";//认证密码
unsigned char sys_password[MAX]="123456";//系统密码


extern unsigned char wData3[20];//存储（一次性临时密码）
extern unsigned char tpc_code_t;//存储（一次性临时密码使用次数）
extern unsigned char *TPC_code;  //临时密码

int i=0;//写入标志位（系统密码）
int j=0;//写入标志位（认证密码）


extern FIL fp;// 文件系统句柄
extern FRESULT res;
extern char rData[4096];
char rData1[128];


extern UINT bw;//多少字节被正常写入
extern UINT br;//多少字节被正常读出

extern int verify_sign;






/******输入一次11，输入认证密码，成功则录取指纹，否则退出****/
/******输入两次11，修改认证密码****/

void handle_FR(void)
	{
		sign=0;
     while(1)
				{
					sign++;
					num=GetKeyVal();
					
					//修改认证密码
					if(num==11)
					{
						rt_kprintf("请输入原认证密码:\n\n");
						del_str(password);
						GET_str(password);
						
						
						//和原认证密码匹配成功，可进行修改
						if(strcmp((const char *)password,(const char *)flash_password)==0)
						{
							rt_kprintf("请设置新的认证密码\n\n");
							del_str(flash_password);
							GET_str(flash_password);
						
							
							//存储修改的认证密码
							res = f_open(&fp,"1:认证密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//配置光标到开头
								res = f_write(&fp,flash_password,sizeof(flash_password),&bw);
								rt_kprintf ("\r\nbw= %d",bw);	
								
								if(res == FR_OK)
								{
									f_lseek(&fp,0);//配置光标到开头
									res = f_read (&fp,rData,f_size(&fp),&br);
									if(res == FR_OK)
										rt_kprintf ("\r\n认证密码：%s br= %d",rData,br);	
								}		
								f_close(&fp);//操作完成，一定要关闭文件
							}
							
								break;
						}
					}
					
					//输入认证密码成功，则录取指纹
					if(sign>50)
					{
						rt_kprintf("请输入认证密码:\n\n");
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
								//验证指纹
								verify_FR();//先判断探测的指纹是否存在，不存在，则提示用户，再刷两次指纹可进行录取；若存在，提醒用户更换指纹，避免指纹重复			

								break;
							}
						else
							{
							rt_kprintf ("认证密码输入错误！！！\n\n");
							break;
							}
					}

					rt_thread_delay(20);
				}
}


/******输入一次13，可以输入系统密码或手机APP下发的临时密码进行解锁，成功则解锁，绿灯亮1秒熄灭，否则退出****/
/******临时密码在使用一次后失效****/
/******输入两次13，修改系统密码****/
void handle_sys(void)
{    
	sign=0;
	
	cJSON *KB;
	char *kb_out;
	uint8_t kb_len;
	
     while(1)
				{
					sign++;

					num=GetKeyVal();
	
				  //手动修改开锁密码
					if(num==13)
					{
						rt_kprintf("请输入原系统密码:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						
						res = f_open(&fp,"1:系统密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);	
						f_lseek(&fp,0);//配置光标到开头
						res = f_read (&fp,sys_password,f_size(&fp),&br);
						rt_kprintf ("\r\n系统密码= %s",sys_password);	
						f_close(&fp);//操作完成，一定要关闭文件
						
						//和原开锁密码匹配成功，可进行修改
						if(strcmp((const char *)openinig_password,(const char *)sys_password)==0)
						{
							rt_kprintf("\n请设置系统密码\n\n");
							del_str(sys_password);
							GET_str(sys_password);
							
							rt_kprintf("系统密码:%s\n\n",sys_password);
							
							//存储修改的开锁密码
							res = f_open(&fp,"1:系统密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						

							if(res == FR_OK)
							{
									f_lseek(&fp,0);//配置光标到开头
									res = f_write(&fp,(unsigned char *)sys_password,sizeof(sys_password),&bw);
									rt_kprintf ("\r\nbw= %d",bw);	
									
									if(res == FR_OK)
									{
										f_lseek(&fp,0);//配置光标到开头
										res = f_read (&fp,rData,f_size(&fp),&br);
										if(res == FR_OK)
											rt_kprintf ("\r\n系统密码：%s br= %d",rData,br);	
									}							
									f_close(&fp);//操作完成，一定要关闭文件
					
						}
			       break;
						}
					}
					
					//解锁
					if(sign>50)
					{
						rt_kprintf("\n请输入系统密码:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						
						//读取系统密码、临时密码
						if(i==1||tpc_code_t=='1')
						{
						   res = f_open(&fp,"1:系统密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
						
								if(res == FR_OK)
								{
									f_lseek(&fp,0);//配置光标到开头
									res = f_read (&fp,rData1,f_size(&fp),&br);
									
									strcpy((char *)sys_password,rData1);
									rt_kprintf("password:%s\n\n",rData1);
									if(res == FR_OK)
										rt_kprintf ("\r\n系统密码：%s br= %d\n\n",sys_password,br);	
								}	
								f_close(&fp);//操作完成，一定要关闭文件

								res = f_open(&fp,"1:一次性临时密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
								rt_kprintf("f_open res =%d\n\n",res);

							if(res == FR_OK)
							{
								f_lseek(&fp,0);//配置光标到开头
								res = f_read (&fp,wData3,f_size(&fp),&br);
								
								tpc_code_t=wData3[6];
								wData3[6]='\0';
								TPC_code=wData3;
								
								if(res == FR_OK)
									{
										rt_kprintf ("临时密码：%s br= %d\n\n",TPC_code,br);	
										rt_kprintf ("临时密码剩余使用次数：%c \n\n",tpc_code_t);	
									}
							}
							f_close(&fp);//操作完成，一定要关闭文件
						
						//判断输入是否为临时密码，成功则失效
						if(strcmp((const char *)openinig_password,(const char *)TPC_code)==0&&tpc_code_t=='1')
							{
								res = f_open(&fp,"1:一次性临时密码.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
									if(res == FR_OK)
										{
											f_lseek(&fp,0);//配置光标到开头
											res = f_write(&fp,"0000000",7,&bw);
											rt_kprintf ("临时密码失效！！！\n\n");
											rt_kprintf ("临时密码剩余使用次数：0 \n\n");	
										}
								f_close(&fp);//操作完成，一定要关闭文件
								
							}
														
						//使用系统密码或临时密码解锁
						if((strcmp((const char *)openinig_password,(const char *)sys_password)==0)||(strcmp((const char *)openinig_password,(const char *)TPC_code)==0&&tpc_code_t=='1'))
						{
							del_str(openinig_password);
							
							RELAY_ON(RELAY0);
							LED2_ON;
							rt_kprintf("开锁成功\n\n");
							
							rt_thread_delay(1000);
						  RELAY_OFF(RELAY0);
							LED2_OFF;
							
							break;
						}
					else
						{
							BEEP=1;//打开蜂鸣器
							LED1_ON;
							rt_thread_delay(100);
							BEEP=0;//关闭蜂鸣器
							rt_thread_delay(1000);
							LED1_OFF;
							rt_kprintf ("系统密码输入错误！！！");
							break;
						}
					}
					
					}
			
					rt_thread_delay(20);
				}
}




