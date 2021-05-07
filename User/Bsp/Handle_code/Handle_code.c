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
unsigned char password[MAX];//ָ��¼������
unsigned char openinig_password[MAX];//��������
unsigned char flash_password[MAX]="12345678";//��֤����
unsigned char sys_password[MAX]="123456";//ϵͳ����


extern unsigned char wData3[20];//�洢��һ������ʱ���룩
extern unsigned char tpc_code_t;//�洢��һ������ʱ����ʹ�ô�����
extern unsigned char *TPC_code;  //��ʱ����

int i=0;//д���־λ��ϵͳ���룩
int j=0;//д���־λ����֤���룩


extern FIL fp;// �ļ�ϵͳ���
extern FRESULT res;
extern char rData[4096];
char rData1[128];


extern UINT bw;//�����ֽڱ�����д��
extern UINT br;//�����ֽڱ���������

extern int verify_sign;






/******����һ��11��������֤���룬�ɹ���¼ȡָ�ƣ������˳�****/
/******��������11���޸���֤����****/

void handle_FR(void)
	{
		sign=0;
     while(1)
				{
					sign++;
					num=GetKeyVal();
					
					//�޸���֤����
					if(num==11)
					{
						rt_kprintf("������ԭ��֤����:\n\n");
						del_str(password);
						GET_str(password);
						
						
						//��ԭ��֤����ƥ��ɹ����ɽ����޸�
						if(strcmp((const char *)password,(const char *)flash_password)==0)
						{
							rt_kprintf("�������µ���֤����\n\n");
							del_str(flash_password);
							GET_str(flash_password);
						
							
							//�洢�޸ĵ���֤����
							res = f_open(&fp,"1:��֤����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						
							if(res == FR_OK)
							{
								f_lseek(&fp,0);//���ù�굽��ͷ
								res = f_write(&fp,flash_password,sizeof(flash_password),&bw);
								rt_kprintf ("\r\nbw= %d",bw);	
								
								if(res == FR_OK)
								{
									f_lseek(&fp,0);//���ù�굽��ͷ
									res = f_read (&fp,rData,f_size(&fp),&br);
									if(res == FR_OK)
										rt_kprintf ("\r\n��֤���룺%s br= %d",rData,br);	
								}		
								f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
							}
							
								break;
						}
					}
					
					//������֤����ɹ�����¼ȡָ��
					if(sign>50)
					{
						rt_kprintf("��������֤����:\n\n");
						del_str(password);
						GET_str(password);
						
						if(j==1)
						{
						  res = f_open(&fp,"1:��֤����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						

							if(res == FR_OK)
							{
								f_lseek(&fp,0);//���ù�굽��ͷ
								res = f_read (&fp,flash_password,f_size(&fp),&br);
								if(res == FR_OK)
									rt_kprintf ("��֤���룺%s br= %d\n\n",flash_password,br);	
							}		
							f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						}
	
						if(strcmp((const char *)password,(const char *)flash_password)==0)
							{
								del_str(password);
								//��ָ֤��
								verify_FR();//���ж�̽���ָ���Ƿ���ڣ������ڣ�����ʾ�û�����ˢ����ָ�ƿɽ���¼ȡ�������ڣ������û�����ָ�ƣ�����ָ���ظ�			

								break;
							}
						else
							{
							rt_kprintf ("��֤����������󣡣���\n\n");
							break;
							}
					}

					rt_thread_delay(20);
				}
}


/******����һ��13����������ϵͳ������ֻ�APP�·�����ʱ������н������ɹ���������̵���1��Ϩ�𣬷����˳�****/
/******��ʱ������ʹ��һ�κ�ʧЧ****/
/******��������13���޸�ϵͳ����****/
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
	
				  //�ֶ��޸Ŀ�������
					if(num==13)
					{
						rt_kprintf("������ԭϵͳ����:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						
						res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);	
						f_lseek(&fp,0);//���ù�굽��ͷ
						res = f_read (&fp,sys_password,f_size(&fp),&br);
						rt_kprintf ("\r\nϵͳ����= %s",sys_password);	
						f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						
						//��ԭ��������ƥ��ɹ����ɽ����޸�
						if(strcmp((const char *)openinig_password,(const char *)sys_password)==0)
						{
							rt_kprintf("\n������ϵͳ����\n\n");
							del_str(sys_password);
							GET_str(sys_password);
							
							rt_kprintf("ϵͳ����:%s\n\n",sys_password);
							
							//�洢�޸ĵĿ�������
							res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);						

							if(res == FR_OK)
							{
									f_lseek(&fp,0);//���ù�굽��ͷ
									res = f_write(&fp,(unsigned char *)sys_password,sizeof(sys_password),&bw);
									rt_kprintf ("\r\nbw= %d",bw);	
									
									if(res == FR_OK)
									{
										f_lseek(&fp,0);//���ù�굽��ͷ
										res = f_read (&fp,rData,f_size(&fp),&br);
										if(res == FR_OK)
											rt_kprintf ("\r\nϵͳ���룺%s br= %d",rData,br);	
									}							
									f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
					
						}
			       break;
						}
					}
					
					//����
					if(sign>50)
					{
						rt_kprintf("\n������ϵͳ����:\n\n");
						del_str(openinig_password);
						GET_str(openinig_password);
						
						//��ȡϵͳ���롢��ʱ����
						if(i==1||tpc_code_t=='1')
						{
						   res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
						
								if(res == FR_OK)
								{
									f_lseek(&fp,0);//���ù�굽��ͷ
									res = f_read (&fp,rData1,f_size(&fp),&br);
									
									strcpy((char *)sys_password,rData1);
									rt_kprintf("password:%s\n\n",rData1);
									if(res == FR_OK)
										rt_kprintf ("\r\nϵͳ���룺%s br= %d\n\n",sys_password,br);	
								}	
								f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�

								res = f_open(&fp,"1:һ������ʱ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
								rt_kprintf("f_open res =%d\n\n",res);

							if(res == FR_OK)
							{
								f_lseek(&fp,0);//���ù�굽��ͷ
								res = f_read (&fp,wData3,f_size(&fp),&br);
								
								tpc_code_t=wData3[6];
								wData3[6]='\0';
								TPC_code=wData3;
								
								if(res == FR_OK)
									{
										rt_kprintf ("��ʱ���룺%s br= %d\n\n",TPC_code,br);	
										rt_kprintf ("��ʱ����ʣ��ʹ�ô�����%c \n\n",tpc_code_t);	
									}
							}
							f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						
						//�ж������Ƿ�Ϊ��ʱ���룬�ɹ���ʧЧ
						if(strcmp((const char *)openinig_password,(const char *)TPC_code)==0&&tpc_code_t=='1')
							{
								res = f_open(&fp,"1:һ������ʱ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
									if(res == FR_OK)
										{
											f_lseek(&fp,0);//���ù�굽��ͷ
											res = f_write(&fp,"0000000",7,&bw);
											rt_kprintf ("��ʱ����ʧЧ������\n\n");
											rt_kprintf ("��ʱ����ʣ��ʹ�ô�����0 \n\n");	
										}
								f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
								
							}
														
						//ʹ��ϵͳ�������ʱ�������
						if((strcmp((const char *)openinig_password,(const char *)sys_password)==0)||(strcmp((const char *)openinig_password,(const char *)TPC_code)==0&&tpc_code_t=='1'))
						{
							del_str(openinig_password);
							
							RELAY_ON(RELAY0);
							LED2_ON;
							rt_kprintf("�����ɹ�\n\n");
							
							rt_thread_delay(1000);
						  RELAY_OFF(RELAY0);
							LED2_OFF;
							
							break;
						}
					else
						{
							BEEP=1;//�򿪷�����
							LED1_ON;
							rt_thread_delay(100);
							BEEP=0;//�رշ�����
							rt_thread_delay(1000);
							LED1_OFF;
							rt_kprintf ("ϵͳ����������󣡣���");
							break;
						}
					}
					
					}
			
					rt_thread_delay(20);
				}
}




