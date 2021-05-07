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


extern unsigned char flash_password[];//��֤����
extern int j;
extern FIL fp;// �ļ�ϵͳ���
extern FRESULT res;
extern UINT br;//�����ֽڱ���������
extern unsigned char password[];



//�߳̿��ƿ�
rt_thread_t FRopen_thread=RT_NULL;//ָ�ƽ����߳̿��ƿ�
rt_thread_t codeOpen_thread=RT_NULL;//��������߳̿��ƿ�

extern u16 num;

//��������߳�
void codeOpen_entry(void *parameter)
{
		while(1)
		{
		
				num=GetKeyVal();
				if(num!=100)
				{
					
					if(num==13)//��һ��13���Խ�����������13�޸�����
					{
							handle_sys();	
					}	
				}
		}
	
}
//ָ�ƽ����߳�
void FRopen_entry(void *parameter)
{
	Handle_AS608();//��ʼ��ָ��ģ�飻ˢ��¼��ɾ����ָ֤��
	Handle_flash();//��ʼ���ļ�ϵͳ����ȡϵͳ���롢��֤���롢��ʱ����
		while(1)
		{
			num=GetKeyVal();
			if(num!=100)
			{
	//			rt_kprintf("num=%d\n\n",num);
				
					if(num==12)
					{
							rt_kprintf("����������֤����:\n\n");
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
									Del_FR();//ɾָ��	
								}else
								{
								rt_kprintf ("��֤����������󣡣���\n\n");
								}
								
						}
					
					
					if(num==11)//��һ��11����¼ȡָ�ƣ�������11���޸���֤����
					{
							handle_FR();			
					}
			}
			
			if(PS_Sta)	 //���PS_Sta״̬���������ָ����
			{
				press_FR();//ˢָ�ƽ���			
			}		
		}
}

