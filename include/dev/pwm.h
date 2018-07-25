#ifndef __PWM_H
#define __PWM_H

#include <stdint.h>
#include <stdbool.h>

#include <stm32f0xx_hal.h>

#define PWM_FREQ 500
#define PWM_MAX_DUTY 1000
#define MAX_PWM_PINS    8
#define DEAFAULT_PRESCALER  (SystemCoreClock / (PWM_MAX_DUTY * PWM_FREQ) - 1)
#define CALC_PRESCALER(freq)  (SystemCoreClock / (PWM_MAX_DUTY * freq) - 1)


#ifdef __cplusplus
 extern "C" {
#endif 

/**
 * 初始化PWM设备
 * @param dutys 初始化占空比（0-1000）
 * @param chn_num pwm通道数量
 */ 
void pwm_init(uint16_t *dutys, uint8_t chn_num);

/**
 * 设置PWM频率
 * @param freq 频率（Hz）
 */ 
void pwm_set_freq(uint16_t freq);

/**
 * 设置占空比
 * @param ch_idx 通道索引（0起）
 * @param duty 占空比 （0-1000）
 */ 
void pwm_set_duty(uint8_t ch_idx, uint16_t duty);

/**
 * 设置所有通道占空比
 * @param duty 占空比 （0-1000）
 */
void pwm_set_all_dutys(uint16_t duty);

/**
 * 启动PWM模块
 * @param fade_in 是否渐亮
 */ 
void pwm_start(bool fade_in);

/**
 * 停止pwm模块
 */  
void pwm_stop();

#ifdef __cplusplus
}
#endif

#endif