//
// Created by HUAWEI on 2020/4/7.
//

#ifndef BASE64_H
#define BASE64_H

//#include <stdint-gcc.h>
#include "stm32f10x.h"

/**
 * base64加密字符串，加密后得到字符串，后面会追加一个字符串的结束符\0
 * @param data 待加密的内容
 * @param dataNum 待加密内容的长度
 * @param result 存储加密后的内容数组，此数组的大小与待加密内容的比例为4：3
 * @param resNum 加密后内容的大小
 */
void Base64_EncodeStr(char *data, int16_t dataNum, char *result, int16_t *resNum);

/**
 * base64加密字节，加密后得到字符串，后面会追加一个字符串的结束符\0
 * @param data 待加密的内容
 * @param dataNum 待加密内容的长度
 * @param result 存储加密后的内容数组，此数组的大小与待加密内容的比例为4：3
 * @param resNum 加密后内容的大小
 */
void Base64_EncodeUChArr(uint8_t *data,int16_t dataNum,char *result,int16_t *resNum);

/**
 * base64解密成字符串，后面会追加一个字符串的结束符\0
 * @param data 待解密的内容
 * @param dataNum 待解密内容的长度
 * @param result 存储解密后的内容数组，此数组的大小与待解密内容的比例为3：4
 * @param resNum 解密后内容的大小
 */
void Base64_DecodeToStr(char *data,int16_t dataNum,char *result,int16_t *resNum);

/**
 * base64解密成字节
 * @param data 待解密的内容
 * @param dataNum 待解密内容的长度
 * @param result 存储解密后的内容数组，此数组的大小与待解密内容的比例为3：4
 * @param resNum 解密后内容的大小
 */
void Base64_DecodeToUChArr(char *data,int16_t dataNum,uint8_t *result,int16_t *resNum);

#endif //BASE64_H
