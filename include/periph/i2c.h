#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <log.h>

#include <stm32f0xx_hal.h>

#define I2C_ADDR (69)

#ifdef __cplusplus
extern "C" {
#endif 

typedef enum
{
  I2C_FREQ_100K = 0,
  I2C_FREQ_400K,
  I2C_FREQ_1000K
} i2c_freq_t;

void i2c_init(uint8_t address);
void i2c_start();
void i2c_stop();

int i2c_set_frequency(i2c_freq_t freq);

int i2c_slave_write(uint8_t bus, uint8_t slave_addr, const uint8_t *data, const uint8_t *buf, uint32_t len);

int i2c_slave_read(uint8_t bus, uint8_t slave_addr, const uint8_t *data, uint8_t *buf, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif