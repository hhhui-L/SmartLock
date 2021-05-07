#include "key.h"
#include "rtthread.h"

/**
** PC8-PC11,列，输出。 PC4-PC7,行，输入
**/
void Key_Config(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
   
	 /********4列输出*********/
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9 | GPIO_Pin_10|GPIO_Pin_11;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
	 
	 /********4行输入*********/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

int KeyScan(void)
{
   int KeyVal=100;
	 GPIO_Write(GPIOC, (GPIOC->ODR & 0xf0ff | 0x0f00));	 //先让PC8-PC11全部输出高。
	if((GPIOC->IDR & 0x00f0)==0x0000)
		  return -1;
	 else
	 {	
//	    delay_ms(20);    //延时5ms去抖动
		 rt_thread_delay(3);
		
	    if((GPIOC->IDR & 0x00f0)==0x0000)
	    return -1;
	 }
	 GPIO_Write(GPIOC,(GPIOC->ODR & 0xf0ff | 0x0100));	//让PC11到PB8输出二进制的0001	 
	 switch(GPIOC->IDR & 0x00f0)
	 {  
		  case 0x0010: KeyVal=3;  break;
			case 0x0020: KeyVal=7;	break;
			case 0x0040: KeyVal=11;	break;
			case 0x0080: KeyVal=15;	break;
   }
	 while((GPIOC->IDR & 0x00f0)	> 0);     //等待按键释放
		
	 GPIO_Write(GPIOC,(GPIOC->ODR & 0xf0ff | 0x0200));	//让PB11到PB7输出二进制的00100.
	 switch(GPIOC->IDR & 0x00f0)		        //对PB12到PB15的值进行判断，以输出不同的键值。
	 {
		  case 0x0010: KeyVal=2;  break;
			case 0x0020: KeyVal=6;	break;
			case 0x0040: KeyVal=10;	break;
			case 0x0080: KeyVal=14;	break;
	 }
   while((GPIOC->IDR & 0x00f0)	> 0);
		
	 GPIO_Write(GPIOC,(GPIOC->ODR & 0xf0ff | 0x0400));	//让PB11到PB7输出二进制的01000.
	 switch(GPIOC->IDR & 0x00f0)		        //对PB12到PB15的值进行判断，以输出不同的键值。
	 {
		  case 0x0010: KeyVal=1;  break;
			case 0x0020: KeyVal=5;	break;
			case 0x0040: KeyVal=9;	break;
			case 0x0080: KeyVal=13;	break;
	 }
	 while((GPIOC->IDR & 0x00f0)	> 0);
	 
	 GPIO_Write(GPIOC,(GPIOC->ODR & 0xf0ff | 0x0800));	//让PB11到PB7输出二进制的10000.
	 switch(GPIOC->IDR & 0x00f0)		        //对PB12到PB15的值进行判断，以输出不同的键值。
	 {
	  	case 0x0010: KeyVal=0;  break;
			case 0x0020: KeyVal=4;	break;
			case 0x0040: KeyVal=8;	break;
			case 0x0080: KeyVal=12;	break;
	 }
	 while((GPIOC->IDR & 0x00f0)	> 0);	 
	 
	 return KeyVal;
}

/************************************
按键表盘为：		1 2 3  S1（11录指纹）
                4 5 6  S2（12删指纹）
                7 8 9  S3（13系统密码）
               OK 0 CL S4（14return）
************************************/
int GetKeyVal(void) 
{
    int num;
    int re;
	  num = KeyScan();
	  switch(num)
	  {    
        case 0:   re=1;     break;				  				      
				case 1:   re=2;     break;					  				       
				case 2:   re=3;     break;					  				       				     
				case 3:   re=11;    break;		    
			                                    
				case 4:   re=4;     break;					  				      
				case 5:   re=5;     break;					  				      
				case 6:   re=6;     break;
			  case 7:   re=12;     break;				
			
			  case 8:  re=7;     break;						 				      		
				case 9:  re=8;     break;					 				      
				case 10:  re=9;     break;							 				      			 				       	
				case 11:  re=13;    break;		    
			
				case 12:  re=20;    break;	      //确定
        case 13:  re=0;     break;							 				      	
			  case 14:  re=21;    break;				//取消		 				       					      
				case 15:  re=14;    break;				
				
			  default:  re=100;   break;
    }
		return re;
}


