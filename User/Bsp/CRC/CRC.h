//
// Created by HUAWEI on 2020/4/6.
//

#ifndef DEMO1_CRC_H
#define DEMO1_CRC_H

#include "stm32f10x.h"

/**
 * 生成CRC校验的值
 * @param ptr 数据
 * @param len 数据长度
 * @return CRC值
 */
uint16_t crc16bitbybit(uint8_t *ptr, uint16_t len);

/**
 * 生成CRC校验的值
 * @param ptr 数据
 * @param len 数据长度
 * @return CRC值
 */
uint16_t crc16table(uint8_t *ptr, uint16_t len);

/**
 * 生成CRC校验的值
 * @param ptr 数据
 * @param len 数据长度
 * @return CRC值
 */
uint16_t crc16tablefast(uint8_t *ptr, uint16_t len);

#endif //DEMO1_CRC_H
