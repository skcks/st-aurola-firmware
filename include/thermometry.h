#ifndef __THERMOMETRY_H
#define __THERMOMETRY_H

#include <stdint.h>

typedef void (*thermometry_callback)(uint8_t, uint8_t);

#ifdef __cplusplus
 extern "C" {
#endif 


/**
 * 初始化
 */ 
void thermometry_init();

/**
 * 读取当前温度
 */  
uint8_t thermometry_read();

/**
 * 设置阈值，包括冷却、超温的度数
 */  
void thermometry_set_threshold(uint8_t cooling_deg, uint8_t fatal_deg);

/**
 * 设置过温回调
 * @param void thermometry_callback(fatal_deg, current_deg)
 */ 
void thermometry_set_excess_callback(thermometry_callback handler);

/**
 * 设置冷却回调
 * @param void thermometry_callback(cooling_deg, current_deg)
 */ 
void thermometry_set_cooling_callback(thermometry_callback handler);


#ifdef __cplusplus
}
#endif

#endif