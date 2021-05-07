//
// Created by HUAWEI on 2020/4/10.
//

#include "myaes.h"
#include "board.h"
#include "rtthread.h"
#include "AES.h"

void dataFiling_pkcs5padding(uint8_t *data,uint16_t *dataLen)
{
    uint8_t a,i;
    i = (*dataLen)%16;
    a = 16-i;
    while (i < 16)
    {
        data[(*dataLen)] = a;
        *dataLen = (*dataLen) + 1;
        i++;
    }
}

void dataRemove_pkcs5padding(uint8_t *data,uint16_t *dataLen)
{
    uint8_t i;
    i = data[*dataLen -1];
    *dataLen = *dataLen - i;
}


void myaes_encrypt(uint8_t *aes_key,uint8_t *data,int16_t datalen,uint8_t *res,int16_t *reslen)
{
    uint8_t datafill[16];
    uint16_t i,j;
    uint8_t *p;
    i = datalen - datalen%16;
    for(j=0;(j+i)<datalen;j++)
    {
        datafill[j] = data[j+i];
    }
		
    dataFiling_pkcs5padding(datafill,&j);
    AES_Init(aes_key,RT_NULL);
		
    if(datalen > 16)
    {
        AES_Encrypt(data,i,res);
    }
    p = res + i;
    AES_Encrypt(datafill,16,p);
    *reslen = i + 16;
}

void myaes_decrypt(uint8_t *aes_key,uint8_t *data,int16_t datalen,uint8_t *res,int16_t *reslen)
{
    uint16_t outsize;
    AES_Init(aes_key,NULL);
    outsize = datalen;
    AES_Decrypt(data,datalen,res);
    dataRemove_pkcs5padding(res,&outsize);
    *reslen = outsize;
}

