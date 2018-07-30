#ifndef __HW_CLOCK_H
#define __HW_CLOCK_H

#include <stdint.h>
#include <stdbool.h>
#include <stm32f0xx_hal.h>

#include "log.h"

#ifdef __cplusplus
 extern "C" {
#endif 


/**
 * 初始化
 */
void hwclock_init();

/**
 * 设置时间戳
 */ 
bool hwclock_set_timestamp(uint32_t val);

/**
 * 读取时间戳
 */  
uint32_t hwclock_get_timestamp();

/**
 * 读取当天已过的秒数
 */ 
uint32_t hwclock_get_seconds_of_day();

#ifdef __cplusplus
}
#endif

#endif