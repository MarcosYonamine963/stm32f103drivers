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
 */
typedef enum
{
    I2C_FREQ_20K = 0,
    I2C_FREQ_50K,
    I2C_FREQ_100K
}i2c_freq_e;

typedef enum
{
    I2C_DATA_DIR_WRITE = 0,
    I2C_DATA_DIR_READ
}i2c_data_dir_e;

typedef enum
{
    I2C_STATUS_OK = 0,
    I2C_STATUS_ERROR,
    I2C_STATUS_BUSY
}i2c_status_e;

void I2C_Config(I2C_TypeDef *I2Cx, i2c_remap_e I2C_REMAP_x, i2c_freq_e I2C_FREQ_x);

i2c_status_e I2C_is_Busy(I2C_TypeDef *I2Cx);

i2c_status_e I2C_Send_Start(I2C_TypeDef *I2Cx, uint8_t slave_addr, i2c_data_dir_e I2C_DATA_DIR_x);

i2c_status_e I2C_Write_Data(I2C_TypeDef *I2Cx, uint8_t data);

i2c_status_e I2C_Read_Data(I2C_TypeDef *I2Cx, uint8_t *data);

i2c_status_e I2C_Send_Stop(I2C_TypeDef *I2Cx);

void I2C_Clear_Received_ACK_flag(I2C_TypeDef *I2Cx);

#endif /* STM32F103DRIVERS_I2C_H_ */
