#ifndef __INSTENSITY_H
#define __INSTENSITY_H

#include "stdint.h"

#ifdef __cplusplus
 extern "C" {
#endif 

/**
 * 初始化亮度模块
 */ 
void instensity_init();

/**
 * 获取亮度值
 * @param channel 通道（0起）
 * @return 亮度值
 */ 
uint16_t instensity_get(uint8_t channel);

/**
 * 设置亮度值
 * @param channel  通道（0起）
 * @param value    亮度值
 * @return 通道序号
 */ 
uint8_t instensity_set(uint8_t channel, uint16_t value);

/**
 * 获取所有通道亮度值
 * @param values    亮度值数组指针
 * @param channel_amount 通道数量
 * @return 通道数量
 */ 
uint8_t instensity_get_all(uint16_t *values, uint8_t channel_amount);

/**
 * 设置所有通道亮度值
 * @param values     亮度值数组指针
 * @param channel_amount  通道数量
 * @return 通道数量
 */ 
uint8_t instensity_set_all(uint16_t *values, uint8_t channel_amount);

/**
 * 获取通道数量
 * @return 通道数量
 */ 
uint8_t instensity_get_amount();

#ifdef __cplusplus
}
#endif

#endif