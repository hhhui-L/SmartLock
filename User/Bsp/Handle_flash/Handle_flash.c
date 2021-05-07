#include "rtthread.h"   
#include "string.h"
#include "stm32f10x.h"
#include "bsp_spi_flash.h"
#include "ff.h"		
#include "Handle_flash.h"
#include "stdio.h"

FATFS fsObject;
FIL fp;// �ļ�ϵͳ���
FRESULT res;


unsigned char wData1[150]="0";//�洢����֤���룩
char wData2[150]="0";//�洢��ϵͳ���룩
unsigned char wData3[20]="0";//�洢��һ������ʱ���룩
unsigned char tpc_code_t=0;//һ������ʱ����ʹ�ô�����1Ϊ��Ч��0Ϊ��Ч
extern unsigned char *TPC_code;  //��ʱ����

char rData[4096]="0";

extern int i;//д���־λ��ϵͳ���룩
extern int j;//д���־λ����֤���룩


extern unsigned char sys_password[];//ϵͳ����
extern unsigned char flash_password[];//��֤����

UINT bw;//�����ֽڱ�����д��
UINT br;//�����ֽڱ���������


void Handle_flash(void)
{
	

		//�����ļ�ϵͳ
		res = f_mount(&fsObject,"1:",1);//"1:"��ʾ·��   1��ʾ��������

		rt_kprintf("f_mount res =%d\n\n",res);
		
		if(res == FR_NO_FILESYSTEM)
		{
			res = f_mkfs("1:",0,0);//��ʽ��,0�Զ�����
			rt_kprintf("f_mkfs res =%d\n\n",res);
			//��ʽ����Ҫȡ�����������¹����ļ�ϵͳ
			res = f_mount(NULL,"1:",1);
			res = f_mount(&fsObject,"1:",1);		
			rt_kprintf("second f_mount res =%d",res);
		}

/**********************�洢ϵͳ����*********************/
		
	  res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		if(res == FR_OK)
		{
			f_lseek(&fp,0);//���ù�굽��ͷ
			res = f_read (&fp,wData2,f_size(&fp),&br);

			if(res == FR_OK)
				rt_kprintf ("ϵͳ���룺%s br= %d\n\n",wData2,br);	

			if(strlen((char *)wData2)<2||strlen((char *)wData2)>12)
			{
				rt_kprintf ("�����޸�ϵͳ���룬����ʹ��ԭʼ���룡����\n\n");
			}
			else
			{
				i=1;
				strcpy((char *)sys_password,(char *)wData2);
				
			}
			
		f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
//	
	}
		
/**********************�洢��֤����*********************/	
	
		res = f_open(&fp,"1:��֤����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		if(res == FR_OK)
		{
			f_lseek(&fp,0);//���ù�굽��ͷ
			res = f_read (&fp,wData1,f_size(&fp),&br);
			
			if(res == FR_OK)
				rt_kprintf ("��֤���룺%s br= %d\n\n",wData1,br);	
			
			if(strlen((char *)wData1)<2||strlen((char *)wData1)>12)
			{
				rt_kprintf ("�����޸���֤���룬����ʹ��ԭʼ���룡����\n\n");
			}
			else
			{
				j=1;
				strcpy((char *)flash_password,(char *)wData1);
				
			}
		}	
		f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
		
		/**********************�洢һ������ʱ����*********************/	
	
		res = f_open(&fp,"1:һ������ʱ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);

		if(res == FR_OK)
		{
			f_lseek(&fp,0);//���ù�굽��ͷ
			res = f_read (&fp,wData3,f_size(&fp),&br);
			
			if(strlen((char *)wData3)>2)
			{
					tpc_code_t=wData3[6];
					wData3[6]='\0';
					TPC_code=wData3;
			}
			
			if(res == FR_OK)
			{
				rt_kprintf ("��ʱ���룺%s br= %d\n\n",TPC_code,br);	
				rt_kprintf ("��ʱ����ʣ��ʹ�ô�����%c \n\n",tpc_code_t);	
			}
			
			if(strlen((char *)TPC_code)<5||tpc_code_t==0)
			{
				rt_kprintf ("һ������ʱ���벻���ڣ�����\n\n");
			}
		}	
		f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
	
}
