#ifndef __HW_CLOCK_H
#define __HW_CLOCK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif 


/**
 * 初始化
 */
void hw_clock_init();

/**
 * 设置时间戳
 */ 
bool hw_clock_set_timestamp(uint32_t val);

/**
 * 读取时间戳
 */  
uint32_t hw_clock_get_timestamp();

/**
 * 读取当天已过的秒数
 */ 
uint32_t hw_clock_get_seconds_of_day();

#ifdef __cplusplus
}
#endif

#endif