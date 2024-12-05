/**
 ******************************************************************************
 * @file      gpio.h
 * @author    Marcos Yonamine
 * @version   v1.0
 * @date      2024-12-05
 *
 * @brief     GPIO Driver for STM32F103 devices
 *
 ******************************************************************************
 */

#ifndef GPIO_H_
#define GPIO_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "stm32f1xx.h"

/**
 * @brief GPIO Modes
 */
typedef enum
{
    OUTPUT_PUSH_PULL = 0,
    OUTPUT_OPEN_DRAIN,
    AF_PUSH_PULL,
    AF_OPEN_DRAIN,
    INPUT_ANALOG,
    INPUT_FLOATING,
    INPUT_PULL_UP,
    INPUT_PULL_DOWN
}gpio_mode_e;

void Gpio_Config(GPIO_TypeDef *GPIO, uint8_t Pin, gpio_mode_e mode);

void Gpio_Digital_Write(GPIO_TypeDef *GPIO, uint8_t Pin, _Bool state);
void Gpio_Digital_Toggle(GPIO_TypeDef *GPIO, uint8_t Pin);
_Bool Gpio_Digital_Read(GPIO_TypeDef *GPIO, uint8_t Pin);

void Gpio_Change_Pin_Mode(GPIO_TypeDef *GPIO, uint8_t Pin, gpio_mode_e mode);

#ifdef  __cplusplus
}
#endif

#endif /* GPIO_H_ */
