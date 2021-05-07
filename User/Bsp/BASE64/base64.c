//
// Created by HUAWEI on 2020/4/7.
//


#include "stm32f10x.h"
#include "base64.h"

const char Base64_Table[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+//";

void Base64_EncodeStr(char *data,int16_t dataNum,char *result,int16_t *resNum)
{
    int16_t i=0,j=0;
    int8_t k,l;
    uint32_t temp;
    while(i<dataNum)
    {
        k=0;
        while(k<3&&i<dataNum)
        {
            temp=temp<<8;
            temp=temp|(uint8_t)data[i];
            i++;
            k++;
        }
        switch(k)
        {
            case 1:
                temp=temp<<16;
                break;
            case 2:
                temp=temp<<8;
                break;
        }
        j=j+3;
        for(l=0;l<4;l++)
        {
            if(k<3)
            {
                temp=temp>>6;
                result[j]='=';
                j--;
                k++;
            }else
            {
                result[j]=Base64_Table[temp&0x3f];
                temp=temp>>6;
                j--;
            }
        }
        j=j+5;
    }
    result[j]='\0';
    *resNum=j;
}

void Base64_EncodeUChArr(uint8_t *data,int16_t dataNum,char *result,int16_t *resNum)
{
    int16_t i=0,j=0;
    int8_t k,l;
    uint32_t temp;
    while(i<dataNum)
    {
        k=0;
        while(k<3&&i<dataNum)
        {
            temp=temp<<8;
            temp=temp|data[i];
            i++;
            k++;
        }
        switch(k)
        {
            case 1:
                temp=temp<<16;
                break;
            case 2:
                temp=temp<<8;
                break;
        }
        j=j+3;
        for(l=0;l<4;l++)
        {
            if(k<3)
            {
                temp=temp>>6;
                result[j]='=';
                j--;
                k++;
            }else
            {
                result[j]=Base64_Table[temp&0x3f];
                temp=temp>>6;
                j--;
            }
        }
        j=j+5;
    }
    result[j]='\0';
    *resNum=j;
}

void Base64_DecodeToStr(char *data,int16_t dataNum,char *result,int16_t *resNum)
{
    uint32_t temp;
    int16_t i=0,j=0;
    int8_t k,l;
    char chTemp;
    while(i<dataNum)
    {
        k=0;
        temp=0;
        while(k<4&&i<dataNum)
        {
            if(data[i]>='A'&&data[i]<='Z')
            {
                temp=temp<<6;
                chTemp=data[i]-'A';
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]>='a'&&data[i]<='z')
            {
                temp=temp<<6;
                chTemp=data[i]-'a'+26;
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]>='0'&&data[i]<='9')
            {
                temp=temp<<6;
                chTemp=data[i]-'0'+52;
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]=='+')
            {
                temp=temp<<6;
                chTemp=62;
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]=='/')
            {
                temp=temp<<6;
                chTemp=63;
                temp=temp|(chTemp&0x3f);
                k++;
            }
            i++;
        }
        switch(k)
        {
            case 2:
                temp=temp>>4;
                result[j]=temp&0xff;
                j++;
                break;
            case 3:
                temp=temp>>2;
                result[j+1]=temp&0xff;
                temp=temp>>8;
                result[j]=temp&0xff;
                j=j+2;
                break;
            case 4:
                j=j+2;
                for(l=0;l<3;l++)
                {
                    result[j]=(char)temp&0xff;
                    j--;
                    temp=temp>>8;
                }
                j=j+4;
                break;
        }
    }
    result[j]='\0';
    *resNum=j;

}

void Base64_DecodeToUChArr(char *data,int16_t dataNum,uint8_t *result,int16_t *resNum)
{
    uint32_t temp;
    int16_t i=0,j=0;
    int8_t k,l;
    char chTemp;
    while(i<dataNum)
    {
        k=0;
        temp=0;
        while(k<4&&i<dataNum)
        {
            if(data[i]>='A'&&data[i]<='Z')
            {
                temp=temp<<6;
                chTemp=data[i]-'A';
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]>='a'&&data[i]<='z')
            {
                temp=temp<<6;
                chTemp=data[i]-'a'+26;
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]>='0'&&data[i]<='9')
            {
                temp=temp<<6;
                chTemp=data[i]-'0'+52;
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]=='+')
            {
                temp=temp<<6;
                chTemp=62;
                temp=temp|(chTemp&0x3f);
                k++;
            }else if(data[i]=='/')
            {
                temp=temp<<6;
                chTemp=63;
                temp=temp|(chTemp&0x3f);
                k++;
            }
            i++;
        }
        switch(k)
        {
            case 2:
                temp=temp>>4;
                result[j]=temp&0xff;
                j++;
                break;
            case 3:
                temp=temp>>2;
                result[j+1]=temp&0xff;
                temp=temp>>8;
                result[j]=temp&0xff;
                j=j+2;
                break;
            case 4:
                j=j+2;
                for(l=0;l<3;l++)
                {
                    result[j]=temp&0xff;
                    j--;
                    temp=temp>>8;
                }
                j=j+4;
                break;
        }
    }
    *resNum=j;

}
