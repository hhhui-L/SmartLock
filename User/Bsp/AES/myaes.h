//
// Created by HUAWEI on 2020/4/10.
//

#ifndef MYAES_H
#define MYAES_H

#include "stm32f10x.h"
#include "AES.h"
#include "board.h"
#include "rtthread.h"

/**
 * 128位AES加密
 * @param aes_key 密钥，指向密钥的指针，密钥大小为16字节
 * @param data 待加密的内容，指向内容的指针
 * @param datalen 待加密内容的长度
 * @param res 存储加密后的内容的数组，16字节的整数倍，大于或者等于待加密的内容
 * @param reslen 加密后的内容的长度
 */
void myaes_encrypt(uint8_t *aes_key,uint8_t *data,int16_t datalen,uint8_t *res,int16_t *reslen);

/**
 * 128位AES解密
 * @param aes_key 密钥，指向密钥的指针，密钥大小为16字节
 * @param data 待解密的内容，指向内容的指针
 * @param datalen 待解密内容的长度
 * @param res 存储解密后的内容的数组
 * @param reslen 解密后的内容的大小
 */
void myaes_decrypt(uint8_t *aes_key,uint8_t *data,int16_t datalen,uint8_t *res,int16_t *reslen);

#endif //MYAES_H
