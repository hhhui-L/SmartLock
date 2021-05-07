#include "G4.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "board.h"
#include "rtthread.h"
#include "newweb.h"

rt_mailbox_t G4_mail = RT_NULL;



struct G4_UART_BUFF G4_GET[3];//存放接收的数据
uint8_t G4_GET_POINT=0;//指向当前存放的区域

struct  G4_USARTx_Fram  G4_Fram_Record = { 0 };

uint32_t StringToInt(char *str)
{
	uint32_t temp=0;
	uint8_t k=0;
	while(*(str+k)!='\0')
	{
		temp=temp*10+*(str+k)-'0';
		k++;
	}
	return temp;
}

char* IntToString(uint32_t integer)
{
	uint32_t temp=1,temp1=integer;
	uint8_t k=1,j=0;
	char *str;
	while((temp*10)<integer)
	{
		temp=temp*10;
		k++;
	}
	str=rt_malloc(sizeof(char)*k);
	for(j=0;j<k;j++)
	{
		*(str+j)=temp1/temp+'0';
		temp1=temp1%temp;
		temp=temp/10;
	}
	*(str+j)='\0';
	return str;
}

void G4_StatePinConfig(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//配置外置时钟，复用映射
	
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 将RST的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = G4_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(G4_RST_PORT, &GPIO_InitStructure);
	
	GPIO_SetBits(G4_RST_PORT, G4_RST_PIN);

  // 将LIKA的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = G4_LIKA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(G4_LIKA_PORT, &GPIO_InitStructure);
	GPIO_EXTILineConfig(G4_LIKA_EXIT_PORTSOURCE,G4_LIKA_EXIT_PINSOURCE);//初始化IO口与中断线的映射关系

	// 将STAT的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = G4_STAT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(G4_STAT_PORT, &GPIO_InitStructure);
	
	//初始化外部中断(exti.c)
	EXTI_InitStruct.EXTI_Line =G4_LIKA_EXTI_LINE;//信号源
	EXTI_InitStruct.EXTI_LineCmd =DISABLE;
	EXTI_InitStruct.EXTI_Mode =EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger =EXTI_Trigger_Falling;//下降沿触发
	EXTI_Init(&EXTI_InitStruct);//调用EXTI_Init库函数
	
	//NVIC初始化设置(misc.c)--定义优先级
	NVIC_InitStruct.NVIC_IRQChannel =G4_LIKA_EXTI_IRQ;//中断源，在stm32f10x.h的IRQn_Type中
	NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =2;//配置抢占优先级：2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority =2;//配置子优先级：2
	NVIC_Init(&NVIC_InitStruct);//调用NVIC_Init库函数

}

/*设置LIKA引脚的中断开关，0关闭，1打开*/
void G4_SetLIKAPin(uint8_t mode)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	if(mode == 1)
	{
		EXTI_InitStruct.EXTI_LineCmd =ENABLE;
	}else
	{
		EXTI_InitStruct.EXTI_LineCmd =DISABLE;
	}
	EXTI_InitStruct.EXTI_Line =G4_LIKA_EXTI_LINE;//信号源
	EXTI_InitStruct.EXTI_Mode =EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger =EXTI_Trigger_Falling;//下降沿触发
	EXTI_Init(&EXTI_InitStruct);
}

/*检查网络状态*/
bool G4_CheckSTAT(void)
{
	uint8_t temp;
	temp=GPIO_ReadInputDataBit(G4_STAT_PORT, G4_STAT_PIN);
	if(temp==0)
	{
		return false;
	}
	return true;
}

/*检查服务器连接状态*/
bool G4_CheckLIKA(void)
{
	uint8_t temp;
	temp=GPIO_ReadInputDataBit(G4_LIKA_PORT, G4_LIKA_PIN);
	if(temp==0)
	{ 
		return false;
	}	
	return true;
}

/*复位模块*/
void G4_RST(void)
{
	uint16_t temp;
	GPIO_ResetBits(G4_RST_PORT, G4_RST_PIN);
	for(temp=0;temp<50000;temp++);
	GPIO_SetBits(G4_RST_PORT, G4_RST_PIN);

}

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = G4_USART_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
void G4_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	G4_USART_GPIO_APBxClkCmd(G4_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	G4_USART_APBxClkCmd(G4_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = G4_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(G4_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = G4_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(G4_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = G4_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(G4_USARTx, &USART_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	
	G4_StatePinConfig();
	
	// 使能串口接收中断
	USART_ITConfig(G4_USARTx, USART_IT_RXNE, ENABLE);	
	//使能空闲
	USART_ITConfig(G4_USARTx, USART_IT_IDLE, ENABLE);	
	
	// 使能串口
	USART_Cmd(G4_USARTx, ENABLE);	    
}

/*
	发送一个字节数据
	*/
void G4_SendByte(uint8_t data)
{
	/* 发送一个字节数据到USART */
	USART_SendData(G4_USARTx,data);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(G4_USARTx, USART_FLAG_TXE) == RESET);	
}

/*
	发送一个字节型数组
	*/
void G4_SendArray(uint8_t *array, uint16_t num)
{
  uint16_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    G4_SendByte(array[i]);	
  
  }
}

/*
	发送字符串
	*/
void G4_SendString( char *str)
{
	unsigned int k=0;
	//rt_kprintf("\r\nsending start\r\n");
  while(*(str+k)!='\0') 
  {
      G4_SendByte( *(str + k) );
      k++;
  }
	//rt_kprintf("\r\nsending end\r\n");
}
/*
 * 函数名：G4_Cmd
 * 描述  ：对4G模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool G4_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	uint32_t delayTime=0;
	G4_Fram_Record.FramLength=0;               //从新开始接收新的数据包
	G4_Fram_Record.FramFinishFlag=0;
	bool sure;//返回值，表示指令执行情况
	//cmdFlag=1;
	G4_SendString (cmd);
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
	{
		return true;
	}
	
	//rt_thread_delay ( waittime );                 //延时
	
	do
	{
		rt_thread_delay(100);						 //延时
		delayTime=delayTime+100;
	}while((delayTime<waittime)&&(G4_Fram_Record.FramFinishFlag==0));
	
	if(G4_Fram_Record.FramFinishFlag==0)
	{
		rt_kprintf("\r\nget nothing\r\n");
		return false;
	}
	G4_Fram_Record .Data_RX_BUF [ G4_Fram_Record .FramLength ]  = '\0';

  rt_kprintf( "%s", G4_Fram_Record .Data_RX_BUF );
	
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		sure = ( ( bool ) strstr ( G4_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( G4_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		sure = ( ( bool ) strstr ( G4_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		sure = ( ( bool ) strstr ( G4_Fram_Record .Data_RX_BUF, reply2 ) );
	return sure;
	
}

/*
	进入AT模式
	*/
bool G4_EnterATMode(void)
{
	bool result=false;
	G4_Cmd ( "+++", NULL, NULL, 0 );
	rt_thread_delay(50);
	G4_Cmd ( "\r\n", NULL, NULL, 0 );
	rt_thread_delay(500);
	result=G4_Cmd ( "AT+VER\r\n", "+OK=", NULL, 5000 );
  rt_kprintf("result=%d\n\n",result);
	return result;
}
/*
	退出AT模式
	*/
bool G4_ExitATMode(void)
{
	bool result=false;
	result=G4_Cmd ( "AT+EXAT\r\n", "+OK", NULL, 5000 );
	return result;
}
/*
	恢复出厂模式
	*/
bool G4_AT_RESTORE(void)
{
	bool result=false;
	result=G4_Cmd ( "AT+RESTORE\r\n", "+", NULL, 500 );
	return result;
}

/*
	重启模块
	*/
bool G4_AT_REBT(void)
{
	bool result=false;
	result=G4_Cmd ( "AT+REBT\r", "+OK", NULL, 500 );
	return result;
}

/*
	查询串口参数
	*/
bool G4_AT_UART_Query(uint32_t *baudrate,uint8_t *parity)
{
	bool result=false;
	char *point,chTemp[10];
	uint8_t k=0;
	result=G4_Cmd ( "AT+UART\r", "+OK=", NULL, 500 );
	if(!result)
	{
		return false;
	}
	point=G4_Fram_Record.Data_RX_BUF+6;
	while(*(point+k)!=',')
	{
		chTemp[k]=*(point+k);
		k++;
	}
	chTemp[k]='\0';
	*baudrate=StringToInt(chTemp);
	if(strstr(G4_Fram_Record.Data_RX_BUF,"NONE"))
	{
		*parity=0;
	}else if(strstr(G4_Fram_Record.Data_RX_BUF,"EVEN"))
	{
		*parity=2;
	}else
	{
		*parity=1;
	}
	return result;
}
/*
	设置串口参数
	*/
bool G4_AT_UART_Set(uint32_t baudrate,char *parity)
{
	bool result=false;
	char chTemp[50],*point;
	strcpy(chTemp,"AT+UART=");
	point=IntToString(baudrate);
	strcat(chTemp,point);
	strcat(chTemp,",");
	strcat(chTemp,parity);
	strcat(chTemp,"\r\n");
	result=G4_Cmd ( chTemp, "+OK", NULL, 500 );
	rt_free(point);
	if(!result)
	{
		return false;
	}
	return result;
}
/*
	查询连接状态
	*/
bool G4_AT_LINKSTA(uint8_t *sta)
{
	bool result=false;
	result=G4_Cmd ( "AT+LINKSTA\r", "+OK=", NULL, 500 );
	if(!result)
	{
		*sta=0;
		return false;
	}
	if(strstr(G4_Fram_Record.Data_RX_BUF,"Connect"))
	{
		*sta=1;
	}else
	{
		*sta=0;
	}
	return result;
}

/*
	设置连接地址
	*/
bool G4_AT_SOCK(char *protocol,char *ip,char *port)
{
	bool result=false;
	char ch[50];
	strcpy(ch,"AT+SOCK=");
	strcat(ch,protocol);
	strcat(ch,",");
	strcat(ch,ip);
	strcat(ch,",");
	strcat(ch,port);
	strcat(ch,"\r");
	result=G4_Cmd ( ch, "+OK", NULL, 500 );
	return result;
}

/*
	设置心跳包模式
	*/
bool G4_AT_HEARTMOD(char *mode)
{
	bool result=false;
	char ch[50];
	strcpy(ch,"AT+HEARTMOD=");
	strcat(ch,mode);
	strcat(ch,"\r");
	result=G4_Cmd ( ch, "+OK", NULL, 500 );
	return result;
}
/*
	设置心跳包数据
	*/
bool G4_AT_HEARTINFO(char *data)
{
	bool result=false;
	char ch[50];
	strcpy(ch,"AT+HEARTINFO=");
	strcat(ch,data);
	strcat(ch,"\r");
	result=G4_Cmd ( ch, "+OK", NULL, 500 );
	return result;
}
/*
	设置心跳包时间
	*/
bool G4_AT_HEARTM(uint32_t time)
{
	bool result=false;
	char ch[50],*point;
	point=IntToString(time);
	strcpy(ch,"AT+HEARTM=");
	strcat(ch,point);
	strcat(ch,"\r");
	result=G4_Cmd ( ch, "+OK", NULL, 500 );
	rt_free(point);
	return result;
}
/**
   *@Brief 是否在连接服务器前清理缓存0否，1是
   *@RetVal 执行结果是否成功
   */
bool G4_AT_UARTCLR(uint8_t flag)
{
	bool result=false;
	char ch[20];
	strcpy(ch,"AT+UARTCLR=");
	if(flag==0)
	{
		strcat(ch,"OFF\r");
	}else
	{
		strcat(ch,"ON\r");
	}
	result=G4_Cmd ( ch, "+OK", NULL, 500 );
	return result;
}


//void G4_USART_IRQHandler()
//{
//	uint8_t chTemp;
//	static uint8_t FirstGet=0;
//	if ( USART_GetITStatus ( G4_USARTx, USART_IT_RXNE ) != RESET )
//	{
//		
//		chTemp=USART_ReceiveData( G4_USARTx ); 
//		
//		rt_kprintf("%c",chTemp);
//		
//		if(G4_Fram_Record.FramLength<( RX_BUF_MAX_LEN - 1 ))
//		{
//			G4_Fram_Record.Data_RX_BUF[G4_Fram_Record.FramLength]=chTemp;
//			G4_Fram_Record.FramLength++;
//		}
//		
//		if(unVanish==1)
//		{
//			if(FirstGet==0)
//			{
//				G4_GET[G4_GET_POINT].num=1;
//				G4_GET[G4_GET_POINT].data[0]=chTemp;
//				FirstGet=1;
//				rt_mb_send(G4_mail,(rt_uint32_t)&G4_GET[G4_GET_POINT]);
//			}else
//			{
//				if(G4_GET[G4_GET_POINT].num<1499)
//				{
//					G4_GET[G4_GET_POINT].data[G4_GET[G4_GET_POINT].num]=chTemp;
//					G4_GET[G4_GET_POINT].num++;
//				}
//				rt_mb_send(G4_mail,(rt_uint32_t)&G4_GET[G4_GET_POINT]);
//			}
//		}
//	}else if ( USART_GetITStatus( G4_USARTx, USART_IT_IDLE ) == SET )
//	{
//		chTemp=USART_ReceiveData( G4_USARTx ); 
//		
//		rt_kprintf("%c",chTemp);
//		
//		if(G4_Fram_Record.FramLength<( RX_BUF_MAX_LEN - 1 ))
//		{
//			G4_Fram_Record.Data_RX_BUF[G4_Fram_Record.FramLength]=chTemp;
//			G4_Fram_Record.FramLength++;
//		}
//		
//		if(unVanish==1)
//		{
//			if(G4_GET[G4_GET_POINT].num<1499)
//			{
//				G4_GET[G4_GET_POINT].data[G4_GET[G4_GET_POINT].num]=chTemp;
//				G4_GET[G4_GET_POINT].num++;
//			}
//			G4_GET[G4_GET_POINT].data[G4_GET[G4_GET_POINT].num]='\0';
//			rt_mb_send(G4_mail,(rt_uint32_t)&G4_GET[G4_GET_POINT]);
//			G4_GET_POINT=(G4_GET_POINT+1)%3;
//		}
//		FirstGet=0;
//	}
//}

void G4_USART_IRQHandler()
{
	uint8_t chTemp;
	static int16_t getnum=0;
	if ( USART_GetITStatus ( G4_USARTx, USART_IT_RXNE ) != RESET )
	{
		chTemp=USART_ReceiveData( G4_USARTx ); 
		//接收到第一个数据字节时，选择接收数据的缓冲区，有3个，每次换一个，防止数据没有被处理就被清掉了
		if(getnum == 0)
		{
			G4_GET_POINT=(G4_GET_POINT+1)%3;
		}
		if(getnum < 1499)//防止接收的数据超过缓冲区的大小，一般不可能发生
		{
			G4_GET[G4_GET_POINT].data[getnum] = chTemp;
			getnum++;
		}
	}else if ( USART_GetITStatus( G4_USARTx, USART_IT_IDLE ) == SET )  //空闲中断，用于判断是否接收完一帧数据
	{
		chTemp=USART_ReceiveData( G4_USARTx ); 
//		if(getnum < 1499)
//		{
//			G4_GET[G4_GET_POINT].data[getnum] = chTemp;
//			getnum++;
//		}
		G4_GET[G4_GET_POINT].num = getnum;
		getnum = 0;
//		rt_kprintf ( "1111111111111\n\n"); 
		rt_mb_send(webmsg_recemb,(rt_uint32_t)&G4_GET[G4_GET_POINT]);
		
	}
}
