#ifndef __HANLE_AS608_H
#define __HANLE_AS608_H

#include <stm32f10x.h>

void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
void press_FR(void);//ˢָ��
void verify_FR(void);//��ָ֤��
void ShowErrMessage(u8 ensure);//��ʾȷ���������Ϣ
void AS608_load_keyboard(u16 x,u16 y,u8 **kbtbl);//�����������
u8  AS608_get_keynum(u16 x,u16 y);//��ȡ������
u16 GET_NUM(void);//��ȡ��ֵ
u16 GET_str(unsigned char *str_num);
void del_str(unsigned char *str_num);
void Handle_AS608(void);

#endif
