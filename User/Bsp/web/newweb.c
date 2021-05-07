#include "newweb.h"
#include "bsp_led.h" 
#include "Handle_code.h"
#include "Handle_flash.h"
#include "bsp_spi_flash.h"
#include "ff.h"	
#include "string.h"
#include "relay.h"

extern FRESULT res;
extern FIL fp;// �ļ�ϵͳ���
extern UINT br;//�����ֽڱ���������
extern UINT bw;//�����ֽڱ�����д��
extern char rData[4096];
extern uint8_t check;
int frequency;

//��֤��Ϣ�������֤��Ϣֻ������dev00001
uint8_t token[]={
	0x02,0x00,0x42,0x7b,0x22,0x64,0x65,0x76,0x69,0x63,0x65,0x4e,0x75,0x6d,0x62,
	0x65,0x72,0x22,0x3a,0x22,0x64,0x65,0x76,0x30,0x30,0x30,0x30,0x31,0x22,0x2c,
	0x22,0x73,0x69,0x67,0x6e,0x61,0x74,0x75,0x72,0x65,0x22,0x3a,0x22,0x6f,0x4a,
	0x72,0x32,0x38,0x31,0x53,0x39,0x51,0x77,0x36,0x48,0x78,0x6f,0x62,0x74,0x38,
	0x73,0x34,0x34,0x74,0x77,0x3d,0x3d,0x22,0x7d,0x76,0x6
};

uint8_t auth_key[16]={0xc6,0xa5,0x7c,0x8c,0xa0,0xc6,0x4e,0x5d,0xd6,0x0c,0xb2,0x3e,0x4f,0x62,0x62,0xe0};//��֤��Կ
uint8_t comm_key[16];//ͨ����Կ

unsigned char *TPC_code;



rt_mq_t webmsg_sendmq=RT_NULL;//�������ݵ���Ϣ����
rt_mailbox_t webmsg_recemb = RT_NULL;//�������ݵ�����

rt_thread_t webmsgsend_th=RT_NULL;   //�������Ӻͷ�����Ϣ�߳�
rt_thread_t webmsgrece_th=RT_NULL;   //������Ϣ���պʹ����߳�

uint8_t conn_flag=0;//�����֤�����ӵ�����˵ı�־��1��ʾ���ӵ������

//��װ���͵����ݣ�ƥ�����ݰ��ĸ�ʽ
void assembleWebMsg(char *msg,int16_t msglen,char *res,int16_t *reslen)
{
	int16_t i;
	uint16_t crc;
	char *p;
	p = res + 3;
	//��������
	myaes_encrypt(comm_key,(uint8_t *)msg,msglen,(uint8_t *)p,&i);
	//������ݰ�ͷ
	res[0] = 0x02;
	res[1] = 0xff&(i>>8);
	res[2] = 0xff&(i);
	i = i + 3;
	//����crcֵ
	crc = crc16tablefast((uint8_t *)res,i);
	//���crc
	res[i++] = 0xff&(crc>>8);
	res[i++] = 0xff&(crc);
	*reslen = i;
}

void newwebconnect_entry(void *parameter)
{
	uint8_t i;
	int16_t caclen;
	char cache[1024];
	char cache2[1024];
	
	uint8_t heart[3] = {0x01,0x80,0x7e};
	rt_err_t err;
	
	while(1)
	{
		rt_kprintf("\n��ʼ���ӷ�����\n");
		G4_RST();
		G4_SetLIKAPin(0);
		rt_thread_delay(2000);
		//���ӷ���˲���
		while(1)
		{
			//����Ƿ����ӷ����
			if(G4_CheckLIKA()==true)
			{
				rt_kprintf("\r\nsuccess connect to web��\r\n");
				break;
			}
			else
			{
				rt_thread_delay(50);
			}
		}
		//������֤��Ϣ
		G4_SendArray(token,71);
		//�ȴ��������֤�����ȴ�5�룬ʱ�������������
		i=0;
		rt_kprintf("\r\n�ȴ���������֤\r\n");
		while(i<50)
		{
			
			rt_thread_delay(100);
			if(conn_flag == 1)
			{
				break;
			}
			i++;
			rt_kprintf("\r\n�ȴ���������֤\r\n");
		}
		//���i��50��˵��û����֤�ɹ���������֤
		if(i >= 50)
		{
			rt_kprintf("\r\n��������֤��ʧ�ܣ�\r\n");
			continue;
		}
		rt_kprintf("\r\n��������֤��ɹ���\r\n");
		//�ȴ������̣߳���������Ҫ���͵�����˵�����
		G4_SetLIKAPin(1);
		while(1)
		{			
			err = rt_mq_recv(webmsg_sendmq,cache,1024,5000);
			if(conn_flag==0)
			{
				rt_kprintf("\n�������ӶϿ�\n");
				break;
			}
			if(err == RT_EOK)
			{
				rt_kprintf("\r\ncache=%s\r\n",cache);
				caclen = rt_strlen(cache);//��������͵����ݳ��ȣ���Ϊ���͵�����ΪJSON��ʽ�����ַ��������Կ�����strlen���㳤��
				if(caclen == 0)
				{
					if(conn_flag==0)
					{
						rt_kprintf("\n�������ӶϿ�\n");
						break;
					}else
					{
						continue;
					}
				}
				assembleWebMsg(cache,caclen,cache2,&caclen);//��װ���͵�����
//				rt_kprintf("\r\ncache2=%s\r\n",cache2);
//				rt_kprintf("\r\ncaclen=%d\r\n",caclen);
				G4_SendArray((uint8_t *)cache2,caclen);//��������
				
			}else
			{
				G4_SendArray(heart,3);  //����������
//				rt_kprintf("\r\nheart\r\n");
			}
			
		}
	}
}

void newwebhandle_entry(void *parameter)
{
	rt_err_t err;
	cJSON *json,*jitem1,*jitem2,*jitem3;
	cJSON *jsend;
	struct G4_UART_BUFF *G4_MSG;
	char * recp;
	char *out;
	int16_t reclen;
	uint8_t cache[1024];
	char cache2[1024];
	int16_t caclen;
	uint8_t len;
	while(1)
	{
		err=rt_mb_recv(webmsg_recemb,(rt_uint32_t *)&G4_MSG,RT_WAITING_FOREVER);
		if(err != RT_EOK)
		{
			rt_kprintf ( "\n�������ʧ��\n\n");
			continue;
		}
		reclen = G4_MSG->num - 5;
		recp = G4_MSG->data + 3;
		if(conn_flag == 0)
		{
			recp[reclen] = '\0';
			rt_kprintf ( "\n\n��֤���صĽ��:%s\n\n",recp);

			//δ�����֤���յ����Ƿ���˷��ص���֤�����Ϣ

			json = cJSON_Parse(recp);
			if(json != RT_NULL)
			{
				jitem1 = cJSON_GetObjectItem(json,"res");
				if((rt_strstr(jitem1->valuestring,"success")))
				{
					jitem2 = cJSON_GetObjectItem(json,"commkey");
//					rt_kprintf ( "\ncommkey:%s\nlength=%d\n\n",jitem2->valuestring,rt_strlen(jitem2->valuestring));
					Base64_DecodeToUChArr(jitem2->valuestring,rt_strlen(jitem2->valuestring),(uint8_t *)cache,&caclen);
					myaes_decrypt(auth_key,(uint8_t *)cache,caclen,cache,&caclen);
					for(reclen=0;reclen<caclen;reclen++)
					{
						comm_key[reclen] = cache[reclen];
					}
					conn_flag = 1;
				}
			}
		}else
		{
			//�����֤���ͨ�����ݣ�ʵ�ʵĶ��豸�Ŀ���ָ��
			myaes_decrypt(comm_key,(uint8_t *)recp,reclen,cache,&caclen);
			cache[caclen] = '\0';
			rt_kprintf ( "\n�յ�����Ϣ:%s\n\n",cache);
			json = cJSON_Parse((char *)cache);
			if(json!=RT_NULL)
			{
				
				jitem1 = cJSON_GetObjectItem(json,"type");
				
				
				/*�ֻ�APP����*/
				if(rt_strstr(jitem1->valuestring,"unlock"))
					{
						
						  RELAY_ON(RELAY0);
						  LED2_ON;
							rt_kprintf("�����ɹ�\n\n");					
							jsend=cJSON_CreateObject();
							cJSON_AddStringToObject(jsend,"type","unlock");
							cJSON_AddStringToObject(jsend,"result","success");
							out=cJSON_PrintUnformatted(jsend);
							rt_kprintf("\nout=%s\n\n",out);
							len=rt_strlen(out);
							len++;
							rt_mq_send(webmsg_sendmq,out,len);
							rt_free(out);
							cJSON_Delete(jsend);
						
							rt_thread_delay(1000);
						
							RELAY_OFF(RELAY0);
							LED2_OFF;

					}
					
					
					/*������ʱ���룬��Ч����Ϊ1*/
					if(rt_strstr(jitem1->valuestring,"setTemporaryCode"))
					{
						char *tpc_code;//��ʱ����
						
						jitem2 = cJSON_GetObjectItem(json,"temporaryCode");
						tpc_code=jitem2->valuestring;
						
						rt_kprintf("\ntpc_code=%s\n\n",tpc_code);
						
						TPC_code=(unsigned char *)tpc_code;
						len=rt_strlen((char *)TPC_code);

						rt_kprintf("\r\nTPC_code =%s\n\n",TPC_code);

					  res = f_open(&fp,"1:һ������ʱ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
						if(res == FR_OK)
						{
							f_lseek(&fp,0);//���ù�굽��ͷ
							res = f_write(&fp,TPC_code,6,&bw);
							res = f_write(&fp,"1",1,&bw);
							
							if(res == FR_OK)
								{
									f_lseek(&fp,0);//���ù�굽��ͷ
									res = f_read (&fp,rData,f_size(&fp),&br);
									if(res == FR_OK)
										rt_kprintf ("\r\n�ļ����ݣ�%s br= %d\r\n",rData,br);	
								}	
						}	
						f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						
						
						//�����������
						jsend=cJSON_CreateObject();
						cJSON_AddStringToObject(jsend,"type","setTemporaryCode");
						cJSON_AddStringToObject(jsend,"result","success");
						out=cJSON_PrintUnformatted(jsend);
						rt_kprintf("\nout=%s\n\n",out);
						len=rt_strlen(out);
						len++;
						rt_mq_send(webmsg_sendmq,out,len);
						rt_free(out);
						cJSON_Delete(jsend);
					}
					
					
					/*�޸�����*/
					if(rt_strstr(jitem1->valuestring,"modifyPassword"))
					{
						char *mfp_code;
						unsigned char MFP_code[100]="0";
						int i=0;
						
						jitem2 = cJSON_GetObjectItem(json,"password");
						mfp_code=jitem2->valuestring;
		
						rt_kprintf("\r\nmfp_code =%s\n\n",mfp_code);
						
						while(*mfp_code!='\0')
						{
							MFP_code[i]=*mfp_code;
							mfp_code++;
							i++;
						}
	
					  res = f_open(&fp,"1:ϵͳ����.txt",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
						if(res == FR_OK)
						{
							f_lseek(&fp,0);//���ù�굽��ͷ
							res = f_write(&fp,MFP_code,sizeof(MFP_code),&bw);
							
							if(res == FR_OK)
								{
									f_lseek(&fp,0);//���ù�굽��ͷ
									res = f_read (&fp,rData,f_size(&fp),&br);
									if(res == FR_OK)
										rt_kprintf ("\r\nϵͳ���룺%s br= %d",rData,br);	
								}	
						}	
						f_close(&fp);//������ɣ�һ��Ҫ�ر��ļ�
						//�����������
						  jsend=cJSON_CreateObject();
							cJSON_AddStringToObject(jsend,"type","modifyPassword");
							cJSON_AddStringToObject(jsend,"result","success");
							out=cJSON_PrintUnformatted(jsend);
							rt_kprintf("\nout=%s\n\n",out);
							len=rt_strlen(out);
							//out[len++]='\0';
							len++;
							rt_mq_send(webmsg_sendmq,out,len);
							rt_free(out);
							cJSON_Delete(jsend);
					}
					
					
					/*�����û�*/
					if(rt_strstr(jitem1->valuestring,"addUser"))
					{
						  char *name;
							jitem2 = cJSON_GetObjectItem(json,"name");
							name=jitem2->valuestring;
						  rt_kprintf("\nname=%s\n\n",name);

						  //�����������
							jsend=cJSON_CreateObject();
							cJSON_AddStringToObject(jsend,"type","addUser");
							cJSON_AddStringToObject(jsend,"result","success");
						  cJSON_AddStringToObject(jsend,"number","1");
							out=cJSON_PrintUnformatted(jsend);
							rt_kprintf("\nout=%s\n\n",out);
							len=rt_strlen(out);
							//out[len++]='\0';
							len++;
							rt_mq_send(webmsg_sendmq,out,len);
							rt_free(out);
							cJSON_Delete(jsend);
						
					}
					
					
					/*�޸��û���*/
					if(rt_strstr(jitem1->valuestring,"modifyUserName"))
					{
						  char *user_number;
							char *user_name;
						  jitem2 = cJSON_GetObjectItem(json,"number");
							jitem3 = cJSON_GetObjectItem(json,"name");
							user_number=jitem2->valuestring;
							user_name=jitem3->valuestring;
						   

						  //�����������
							jsend=cJSON_CreateObject();
							cJSON_AddStringToObject(jsend,"type","modifyUserName");
							cJSON_AddStringToObject(jsend,"result","success");
							out=cJSON_PrintUnformatted(jsend);
							rt_kprintf("\nout=%s\n\n",out);
							len=rt_strlen(out);
							len++;
							rt_mq_send(webmsg_sendmq,out,len);
							rt_free(out);
							cJSON_Delete(jsend);
						
					}
					
					
					/*ɾ���û�*/
					if(rt_strstr(jitem1->valuestring,"deleteUser"))
					{
						  char *number;
							jitem2 = cJSON_GetObjectItem(json,"number");
							number=jitem2->valuestring;
					  	rt_kprintf("\nnumber=%s\n\n",number);
						   

						  //����������� 
							jsend=cJSON_CreateObject();
							cJSON_AddStringToObject(jsend,"type","deleteUser");
							cJSON_AddStringToObject(jsend,"result","success");
							out=cJSON_PrintUnformatted(jsend);
							rt_kprintf("\nout=%s\n\n",out);
							len=rt_strlen(out);
							//out[len++]='\0';
							len++;
							rt_mq_send(webmsg_sendmq,out,len);
							rt_free(out);
							cJSON_Delete(jsend);
						
					}
			}
			
	 }
 }
}

