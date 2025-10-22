/**
 * @file i2c.h
 * @brief I2C Driver for STM32F103 Microcontrollers
 * @author Marcos Yonamine
 */

#ifndef STM32F103DRIVERS_I2C_H_
#define STM32F103DRIVERS_I2C_H_

#include "stm32f1xx.h"

typedef enum
{
    I2C_REMAP_NO_REMAP = 0,
    I2C_REMAP_REMAP
}i2c_remap_e;

/**
 * @brief I2C Frequency options. The lower the frequency, the higher the reliability.
 *
 */
typedef enum
{
    I2C_FREQ_20K = 0,
    I2C_FREQ_50K,
    I2C_FREQ_100K,
    I2C_FREQ_400K
}i2c_freq_e;

/**
 * @brief I2C return status code
 *
 */
typedef enum
{
    I2C_STATUS_OK = 0,
    I2C_STATUS_ERROR,
    I2C_STATUS_BUSY
}i2c_status_e;

void I2C_Config(I2C_TypeDef *I2Cx, i2c_remap_e I2C_REMAP_x, i2c_freq_e I2C_FREQ_x);
i2c_status_e I2C_Write_Data_Array(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t data[], uint16_t len);
i2c_status_e I2C_Read_Data_Array(I2C_TypeDef *I2Cx, uint8_t slave_addr, uint8_t data[], uint16_t len);
i2c_status_e I2C_is_Busy(I2C_TypeDef *I2Cx);

#endif /* STM32F103DRIVERS_I2C_H_ */
