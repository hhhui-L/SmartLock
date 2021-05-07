#ifndef _AES_H_
#define _AES_H_
#include "stm32f10x.h"
#include <stdbool.h>
#include "board.h"
#include "rtthread.h"


#define u8          uint8_t
#define u16         uint16_t

#define AES_KEY_LENGTH	  16//秘钥,必须为16,24,32字节(128 192 256位),可以为数字数组
#define AES_IV_LENGTH	  16//分组长度必须16直接，初始化向量也必须为16字节,可以为数字数组
#define BPOLY           0x1B//GF(28)多项式 0x1B=00011011 (x^4 + x^3 + x + 1)

/*
AES	      密钥长度 	分组长度(AES_IV长度) 	加密轮数
AES-128      16           16	               10
AES-192	     24	          16	               12
AES-256      32           16	               14
*/
// 加解密模式
#define AES_MODE_ECB	0				// 电子密码本模式（一般模式）
#define AES_MODE_CBC	1				// 密码分组链接模式
#define AES_MODE		AES_MODE_ECB

#define Nk	(AES_KEY_LENGTH / 4)		// 以“字”（4字节）为单位的密钥长度
#define Nb	                  4			// 以“字”（4字节）为单位的加解密数据块大小，固定为4

// Nr：加密的轮数
#if   AES_KEY_LENGTH == 16
	#define Nr	10
#elif AES_KEY_LENGTH == 24
	#define Nr	12
#elif AES_KEY_LENGTH == 32
	#define Nr	14
#else
	#error AES_KEY_LENGTH must be 16, 24 or 32 BOOLs!
#endif
 
///////////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Init
//	描述：		初始化，在此执行扩展密钥操作。
//	输入参数：	pKey  -- 原始密钥  ，其长度必须为 AES_KEY_LENGTH字节。
//	输入参数：	pIV   -- 初始化向量，其长度必须为 AES_IV_LENGTH字节。
///////////////////////////////////////////////////////////////////////////////
void AES_Init(u8 *pKey,u8 *pIV);

//////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Encrypt
//	描述：		加密数据
//	输入参数：	pPlainText	-- 明文，即需加密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH的整倍数。
//	输出参数：	pCipherText	-- 密文，即由明文加密后的数据，可以与pPlainText相同。
//////////////////////////////////////////////////////////////////////////
void AES_Encrypt(u8 *pPlainText,u16 nDataLen,u8 *pCipherText);
void AES_Encrypt_Oneself(u8 *pPlainText,u16 nDataLen); //加密到原数组

//////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Decrypt
//	描述：		解密数据
//	输入参数：	pCipherText -- 密文，即需解密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH的整倍数。
//	输出参数：	pPlainText  -- 明文，即由密文解密后的数据，可以与pCipherText相同。
//////////////////////////////////////////////////////////////////////////
void AES_Decrypt(u8 *pCipherText,u16 nDataLen,u8 *pPlainText);
void AES_Decrypt_Oneself(u8 *pCipherText,u16 nDataLen); //解密到原数组

void AES_TEST(void);

#endif
