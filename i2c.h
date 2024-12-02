#ifndef STM32F103DRIVERS_I2C_H_
#define STM32F103DRIVERS_I2C_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "stm32f1xx.h"

/*
Config I2C1 Remap:

I2C1_PIN_REMAP_NO:
    SCL: PB6
    SDA: PB7

I2C1_PIN_REMAP:
    SCL: PB8
    SDA: PB9
*/
typedef enum
{
    I2C1_PIN_REMAP_NO = 0,
    I2C1_PIN_REMAP,
}i2c1_pin_remap_e;

void I2C1_Config(i2c1_pin_remap_e I2C1_PIN_REMAPx);
uint8_t I2C1_Scan_Bus(uint8_t *addr);






#ifdef  __cplusplus
}
#endif

#endif /* STM32F103DRIVERS_I2C_H_ */
