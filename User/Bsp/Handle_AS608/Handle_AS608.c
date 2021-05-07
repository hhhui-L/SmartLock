#include "board.h"
#include "rtthread.h"
#include "key.h"
#include "bsp_usart.h"
#include "beep.h"
#include "sys.h"
#include "usart2.h"
#include "AS608.h"
#include "timer.h"
#include "Handle_as608.h"
#include "stdio.h"
#include "string.h"
#include "newweb.h"
#include "relay.h"

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
//u8** kbd_tbl;
int num=100;
int verify_sign=0;

//��ȡ������ֵ
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=GetKeyVal();	
		if(key_num!=100)
		{
			if(key_num==13)return 0xFFFF; //�����ء���
			if(key_num==14)return 0xFF00;  //		
			if(key_num>0&&key_num<=9&&num<9999)//��1-9����(��������3λ��)
				num =num*10+key_num;		
			if(key_num==21)num =num/10;//��Del����			
			if(key_num==0&&num<9999)num =num*10;//��0����
			if(key_num==20)return num;  //��Enter����
			
			rt_kprintf("num=%d\n\n",num);
		}
		
	}	
}


u16 GET_str(unsigned char *str_num)
{
	char  key_num=0;
//	u16 num=0;
	unsigned char count=0;
	while(1)
	{
		key_num=GetKeyVal();	
		if(key_num!=100)
		{
			
			if(key_num==13)return 0xFFFF; //�����ء���
			if(key_num==14)return 0xFF00;  //			
			if(key_num>=0&&key_num<=9)//��1-9����(��������3λ��)
			{
				str_num[count++] = key_num+'0';
				str_num[count] = '\0';
			}		
			if(key_num==21)str_num[--count]='\0';//��Del����					
			if(key_num==20)return 0xF000;  //��Enter����
			
			rt_kprintf("str_num=%s\n\n",str_num);
		}
	}	
	
	
}




void del_str(unsigned char *str_num)
{
	unsigned char count;
	u16 length=0;
	length = strlen((const char *)str_num);
	for(count=0;count<length;count++)
		str_num[count] = '\0';
}


//¼ָ��
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
	u16 count=0;
	
	rt_kprintf("��ʼ¼ָ�ƣ�����\n\n");
	
	while(1)
	{
		count++;
		switch (processnum)
		{
			case 0:
				i++;
				rt_kprintf("�밴ָ��\n\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						rt_kprintf("ָ������\n\n");
						i=0;
						processnum=1;//�����ڶ���						
					}else rt_kprintf("ָ�ƴ���\n\n");//ShowErrMessage(ensure);				
				}else rt_kprintf("������������\n\n");//ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				rt_kprintf("�밴�ٰ�һ��ָ��\n\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//��������
					BEEP=0;
					if(ensure==0x00)
					{
						rt_kprintf("ָ������\n\n");
						i=0;
						processnum=2;//����������
					}else rt_kprintf("ָ�ƴ���\n\n");// ShowErrMessage(ensure);	
				}else rt_kprintf("������������\n\n");//ShowErrMessage(ensure);		
				break;

			case 2:
				rt_kprintf("�Ա�����ָ��\n\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					rt_kprintf("�Աȳɹ�,����ָ��һ��\n\n");
					processnum=3;//�������Ĳ�
				}
				else 
				{
					rt_kprintf("�Ա�ʧ�ܣ�������¼��ָ��\n\n");
					i=0;
					processnum=0;//���ص�һ��		
				}
				rt_thread_delay(30);
				break;

			case 3:
				rt_kprintf("����ָ��ģ��\n\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
				rt_kprintf("����ָ��ģ��ɹ�\n\n");
					processnum=4;//�������岽
				}
				else 
				{
					processnum=0;
				rt_kprintf("����ָ��ģ�����\n\n");
				}
				rt_thread_delay(30);
				break;
				
			case 4:	
				rt_kprintf("�����봢��ID,��Enter����\n\n");
				rt_kprintf("0=< ID <=299\n\n");
			
				do
				{
					ID=GET_NUM();
				}
				while(!(ID<AS608Para.PS_max));//����ID����С��ָ�����������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
					rt_kprintf("¼��ָ�Ƴɹ�\n\n");		
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					rt_kprintf("AS608Para.PS_max-ValidN=%d\n\n",AS608Para.PS_max-ValidN);
					rt_thread_delay(30);
					return ;
				}else {processnum=0;rt_kprintf("¼��ָ��ʧ��\n\n");}					
				break;				
		}
		rt_thread_delay(500);

		if(count==10)//����5��û�а���ָ���˳�
		{
			rt_kprintf("����5��û�а���ָ���˳�������\n\n");
			break;	
		}			
	}
}

//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	cJSON *FR;
	char *fr_out;
	uint8_t fr_len;

	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				rt_kprintf("ˢָ�Ƴɹ�\n\n");				
				str=rt_malloc(50);
				
				RELAY_ON(RELAY0);
				LED2_ON;
				sprintf(str,"ȷ�д���,ID:%d  ƥ��÷�:%d �����ɹ�\n\n",seach.pageID,seach.mathscore);
				
//				FR=cJSON_CreateObject();
//				cJSON_AddStringToObject(FR,"type","unlock");
//				cJSON_AddStringToObject(FR,"result","success");
//				fr_out=cJSON_PrintUnformatted(FR);
//				rt_kprintf("\nfr_out=%s\n\n",fr_out);
//				fr_len=rt_strlen(fr_out);
//				//out[len++]='\0';
//				fr_len++;
//				rt_mq_send(webmsg_sendmq,fr_out,fr_len);
//				rt_free(fr_out);
//				cJSON_Delete(FR);
				
				rt_thread_delay(1000);
			  RELAY_OFF(RELAY0);
				LED2_OFF;
				
				rt_kprintf("str=%s\n\n",str);	
				rt_free(str);
			}
			else
				{	
					BEEP=1;//�򿪷�����
					LED1_ON;
					rt_thread_delay(100);
					BEEP=0;//�رշ�����
					rt_thread_delay(1000);
					LED1_OFF;
					rt_kprintf("����ʧ�ܣ�����ʧ�ܣ�\n\n");
				}
									
	  }
		else
     rt_kprintf("��������ʧ��\n\n");
	 BEEP=0;//�رշ�����
	 rt_thread_delay(600);

	}
}

//��ָ֤��
void verify_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	int i=0;
	while(1)
	{
		i++;
	 if(PS_Sta)	 //���PS_Sta״̬���������ָ����
	 {
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				rt_kprintf("ˢָ�Ƴɹ�\n\n");				
				str=rt_malloc(50);
				sprintf(str,"����ָ��,ID:%d  ƥ��÷�:%d\n\n�뻻��ָ��!!!\n\n",seach.pageID,seach.mathscore);
				
				BEEP=1;//�򿪷�����
				LED3_ON;
				rt_thread_delay(50);
				BEEP=0;//�رշ�����
				rt_thread_delay(200);
				LED3_OFF;
				
				rt_kprintf("str=%s\n\n",str);	
				rt_free(str);
				verify_sign=1;
				if(i==6)
				{
				break;	
				}
			}
			else 
			{
				verify_sign=0;
				rt_kprintf("��ָ��δ¼�룡����\n\n");		
				Add_FR();	//¼ָ��
				break;	
			}				
	  }
		else
		{
     rt_kprintf("����ָ������ʧ�ܣ��˳�������\n\n");
			break;	
		}
	 BEEP=0;//�رշ�����
	 rt_thread_delay(500);
	}
	}
	 
	   rt_kprintf("�밴ָ�ƣ�����\n\n");
			if(i==10)//����10��û�а���ָ���˳�
		{
			verify_sign=2;
			rt_kprintf("����10��û�а���ָ���˳�������\n\n");
			break;	
		}	
	rt_thread_delay(2000);
 }
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 ID;
	rt_kprintf("ɾ��ָ��\n\n");
	rt_kprintf("������ָ��ID��Enter����\n\n");
	rt_kprintf("0=< ID <=299\n\n");
	rt_thread_delay(12);

	ID=GET_NUM();//��ȡ���ص���ֵ

		if(ID==0xFFFF)
	{
		rt_kprintf("����������\n\n");
	}
	
	if(ID==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(ID,1);//ɾ������ָ��
	
	if(ensure==0)
	{
		rt_kprintf("ɾ��ָ�Ƴɹ�!!!\n\n");
	
	}
  else
		rt_kprintf("ɾ��ָ�Ʋ��ɹ�!!!\n\n");
	
	rt_thread_delay(300);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	rt_kprintf("AS608Para.PS_max-ValidN=%d\n\n",AS608Para.PS_max-ValidN);

	rt_thread_delay(50);
}


//ָ��ģ��ͨ��
void Handle_AS608(void)
{
	  u8 ensure;
	  char *str;	

  rt_kprintf("��AS608ģ������....\n\n");	
	
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		rt_thread_delay(100);
		rt_kprintf("δ��⵽ģ�飡����\n\n");
		rt_thread_delay(200);
		rt_kprintf("��������ģ��...\n\n");	  
	}

	rt_kprintf("ͨѶ�ɹ�!!!\n\n");
	str=rt_malloc(30);
	sprintf(str,"������:%d   ��ַ:%x",usart2_baund,AS608Addr);
	rt_kprintf("%s\n\n",str);
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		rt_kprintf("OK\n\n");//ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	if(ensure==0x00)
	{
		rt_memset(str,0,50);
		sprintf(str,"������:%d     �Աȵȼ�: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		rt_kprintf("%s\n\n",str);
	}
	else
		rt_kprintf("ERROR\n\n");	
	rt_free(str);


}
