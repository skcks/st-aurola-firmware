#ifndef __FAN_H
#define __FAN_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 

/**
 * 初始化风扇模块
 */
void fan_init();

/**
 * 检测风扇速度
 * @return true 为已完成测速， false 为未完成
 */ 
bool fan_detect_speed();

/**
 * 读取风扇速度
 * @return 风扇速度
 */  
uint16_t fan_get_speed();

/**
 * 设置风扇速度
 * @param duty 速度百分比
 */ 
void fan_set_speed(uint16_t duty);

/**
 * 关闭风扇
 */  
void fan_off();
/**
 * 开启风扇
 */  
void fan_on();

#ifdef __cplusplus
}
#endif

#endif