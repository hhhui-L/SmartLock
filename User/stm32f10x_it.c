/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "G4.h"
#include "board.h"
#include "rtthread.h"
#include "newweb.h"

void EXTI1_IRQHandler()//�жϷ�����stm3210x.h   startuo_stm32f10x_hd.s���ҵ�
{
	char out = '\0';
	if(EXTI_GetITStatus(G4_LIKA_EXTI_LINE) != RESET)
	{
		rt_kprintf("test1111\n\n");
		if(conn_flag==1)
		{
			G4_SetLIKAPin(0);
			conn_flag = 0;
			rt_mq_send(webmsg_sendmq,&out,1);
			rt_kprintf("test2222\n\n");
		}
		EXTI_ClearITPendingBit(G4_LIKA_EXTI_LINE);//����жϱ�־λ	
		EXTI_ClearFlag(G4_LIKA_EXTI_LINE);
	}
}
