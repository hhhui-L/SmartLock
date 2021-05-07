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

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数
//u8** kbd_tbl;
int num=100;
int verify_sign=0;

//获取键盘数值
u16 GET_NUM(void)
{
	u8  key_num=0;
	u16 num=0;
	while(1)
	{
		key_num=GetKeyVal();	
		if(key_num!=100)
		{
			if(key_num==13)return 0xFFFF; //‘返回’键
			if(key_num==14)return 0xFF00;  //		
			if(key_num>0&&key_num<=9&&num<9999)//‘1-9’键(限制输入3位数)
				num =num*10+key_num;		
			if(key_num==21)num =num/10;//‘Del’键			
			if(key_num==0&&num<9999)num =num*10;//‘0’键
			if(key_num==20)return num;  //‘Enter’键
			
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
			
			if(key_num==13)return 0xFFFF; //‘返回’键
			if(key_num==14)return 0xFF00;  //			
			if(key_num>=0&&key_num<=9)//‘1-9’键(限制输入3位数)
			{
				str_num[count++] = key_num+'0';
				str_num[count] = '\0';
			}		
			if(key_num==21)str_num[--count]='\0';//‘Del’键					
			if(key_num==20)return 0xF000;  //‘Enter’键
			
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


//录指纹
void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
	u16 count=0;
	
	rt_kprintf("开始录指纹！！！\n\n");
	
	while(1)
	{
		count++;
		switch (processnum)
		{
			case 0:
				i++;
				rt_kprintf("请按指纹\n\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer1);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						rt_kprintf("指纹正常\n\n");
						i=0;
						processnum=1;//跳到第二步						
					}else rt_kprintf("指纹错误\n\n");//ShowErrMessage(ensure);				
				}else rt_kprintf("生成特征错误\n\n");//ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				rt_kprintf("请按再按一次指纹\n\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP=1;
					ensure=PS_GenChar(CharBuffer2);//生成特征
					BEEP=0;
					if(ensure==0x00)
					{
						rt_kprintf("指纹正常\n\n");
						i=0;
						processnum=2;//跳到第三步
					}else rt_kprintf("指纹错误\n\n");// ShowErrMessage(ensure);	
				}else rt_kprintf("生成特征错误\n\n");//ShowErrMessage(ensure);		
				break;

			case 2:
				rt_kprintf("对比两次指纹\n\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					rt_kprintf("对比成功,两次指纹一样\n\n");
					processnum=3;//跳到第四步
				}
				else 
				{
					rt_kprintf("对比失败，请重新录入指纹\n\n");
					i=0;
					processnum=0;//跳回第一步		
				}
				rt_thread_delay(30);
				break;

			case 3:
				rt_kprintf("生成指纹模板\n\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
				rt_kprintf("生成指纹模板成功\n\n");
					processnum=4;//跳到第五步
				}
				else 
				{
					processnum=0;
				rt_kprintf("生成指纹模板错误\n\n");
				}
				rt_thread_delay(30);
				break;
				
			case 4:	
				rt_kprintf("请输入储存ID,按Enter保存\n\n");
				rt_kprintf("0=< ID <=299\n\n");
			
				do
				{
					ID=GET_NUM();
				}
				while(!(ID<AS608Para.PS_max));//输入ID必须小于指纹容量的最大值
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
				if(ensure==0x00) 
				{			
					rt_kprintf("录入指纹成功\n\n");		
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					rt_kprintf("AS608Para.PS_max-ValidN=%d\n\n",AS608Para.PS_max-ValidN);
					rt_thread_delay(30);
					return ;
				}else {processnum=0;rt_kprintf("录入指纹失败\n\n");}					
				break;				
		}
		rt_thread_delay(500);

		if(count==10)//超过5次没有按手指则退出
		{
			rt_kprintf("超过5次没有按手指，退出！！！\n\n");
			break;	
		}			
	}
}

//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	cJSON *FR;
	char *fr_out;
	uint8_t fr_len;

	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		BEEP=1;//打开蜂鸣器	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			BEEP=0;//关闭蜂鸣器	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
			{				
				rt_kprintf("刷指纹成功\n\n");				
				str=rt_malloc(50);
				
				RELAY_ON(RELAY0);
				LED2_ON;
				sprintf(str,"确有此人,ID:%d  匹配得分:%d 开锁成功\n\n",seach.pageID,seach.mathscore);
				
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
					BEEP=1;//打开蜂鸣器
					LED1_ON;
					rt_thread_delay(100);
					BEEP=0;//关闭蜂鸣器
					rt_thread_delay(1000);
					LED1_OFF;
					rt_kprintf("搜索失败！解锁失败！\n\n");
				}
									
	  }
		else
     rt_kprintf("生成特征失败\n\n");
	 BEEP=0;//关闭蜂鸣器
	 rt_thread_delay(600);

	}
}

//验证指纹
void verify_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	int i=0;
	while(1)
	{
		i++;
	 if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
	 {
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		BEEP=1;//打开蜂鸣器	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			BEEP=0;//关闭蜂鸣器	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
			{				
				rt_kprintf("刷指纹成功\n\n");				
				str=rt_malloc(50);
				sprintf(str,"已有指纹,ID:%d  匹配得分:%d\n\n请换个指纹!!!\n\n",seach.pageID,seach.mathscore);
				
				BEEP=1;//打开蜂鸣器
				LED3_ON;
				rt_thread_delay(50);
				BEEP=0;//关闭蜂鸣器
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
				rt_kprintf("此指纹未录入！！！\n\n");		
				Add_FR();	//录指纹
				break;	
			}				
	  }
		else
		{
     rt_kprintf("生成指纹特征失败，退出！！！\n\n");
			break;	
		}
	 BEEP=0;//关闭蜂鸣器
	 rt_thread_delay(500);
	}
	}
	 
	   rt_kprintf("请按指纹！！！\n\n");
			if(i==10)//超过10次没有按手指则退出
		{
			verify_sign=2;
			rt_kprintf("超过10次没有按手指，退出！！！\n\n");
			break;	
		}	
	rt_thread_delay(2000);
 }
}

//删除指纹
void Del_FR(void)
{
	u8  ensure;
	u16 ID;
	rt_kprintf("删除指纹\n\n");
	rt_kprintf("请输入指纹ID按Enter发送\n\n");
	rt_kprintf("0=< ID <=299\n\n");
	rt_thread_delay(12);

	ID=GET_NUM();//获取返回的数值

		if(ID==0xFFFF)
	{
		rt_kprintf("返回主界面\n\n");
	}
	
	if(ID==0xFF00)
		ensure=PS_Empty();//清空指纹库
	else 
		ensure=PS_DeletChar(ID,1);//删除单个指纹
	
	if(ensure==0)
	{
		rt_kprintf("删除指纹成功!!!\n\n");
	
	}
  else
		rt_kprintf("删除指纹不成功!!!\n\n");
	
	rt_thread_delay(300);
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	rt_kprintf("AS608Para.PS_max-ValidN=%d\n\n",AS608Para.PS_max-ValidN);

	rt_thread_delay(50);
}


//指纹模块通信
void Handle_AS608(void)
{
	  u8 ensure;
	  char *str;	

  rt_kprintf("与AS608模块握手....\n\n");	
	
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		rt_thread_delay(100);
		rt_kprintf("未检测到模块！！！\n\n");
		rt_thread_delay(200);
		rt_kprintf("尝试连接模块...\n\n");	  
	}

	rt_kprintf("通讯成功!!!\n\n");
	str=rt_malloc(30);
	sprintf(str,"波特率:%d   地址:%x",usart2_baund,AS608Addr);
	rt_kprintf("%s\n\n",str);
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		rt_kprintf("OK\n\n");//ShowErrMessage(ensure);//显示确认码错误信息	
	ensure=PS_ReadSysPara(&AS608Para);  //读参数 
	if(ensure==0x00)
	{
		rt_memset(str,0,50);
		sprintf(str,"库容量:%d     对比等级: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		rt_kprintf("%s\n\n",str);
	}
	else
		rt_kprintf("ERROR\n\n");	
	rt_free(str);


}
