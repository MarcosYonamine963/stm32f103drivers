/**
 * @file gpio.c
 */

#include "gpio.h"

/*
 * @brief Enable peripheral clock for the selected GPIO Port
 * @param GPIO [input]: GPIOA ~ GPIOC
 * */
static void _gpio_config_clock(GPIO_TypeDef *GPIO)
{

    /* Enable corresponding clock */
    switch( (uint32_t)GPIO )
    {
        case (uint32_t)GPIOA:
            // Enable APB2 GPIOA clock
            RCC->APB2ENR |= (1<<2);
        break;

        case (uint32_t)GPIOB:
            // Enable APB2 GPIOB clock
            RCC->APB2ENR |= (1<<3);
        break;

        case (uint32_t)GPIOC:
            // Enable APB2 GPIOC clock
            RCC->APB2ENR |= (1<<4);
        break;

        default:
            return;
    }// end switch GPIO

}// end _gpio_config_clock


/**
 * @brief Configure GPIO port, Pin and mode
 *
 * Configure the GPIO clock, and the pin mode.
 *
 * @param GPIO GPIOA~GPIOC
 * @param Pin 0~15
 * @param mode see enum gpio_mode_e
 */
void Gpio_Config(GPIO_TypeDef *GPIO, uint8_t Pin, gpio_mode_e mode)
{
    /* Enable corresponding clock */
   _gpio_config_clock(GPIO);

   Gpio_Change_Pin_Mode(GPIO, Pin, mode);

}// end Gpio_Config

/**
 * @brief Change the GPIO Pin mode (must be already configured)
 *
 * @param GPIO GPIOA~GPIOC
 * @param Pin 0~15
 * @param mode see enum gpio_mode_e
 */
void Gpio_Change_Pin_Mode(GPIO_TypeDef *GPIO, uint8_t Pin, gpio_mode_e mode)
{

    uint32_t mode_mask = 0;     // bit mask for set CNF and MODE bits on CRL or CRH register
                                // see section 9.2 on reference manual

    switch(mode)
    {
        case OUTPUT_OPEN_DRAIN:
            mode_mask = 0b0111;
            break;
        case OUTPUT_PUSH_PULL:
            mode_mask = 0b0011;
            break;
        case INPUT_ANALOG:
            mode_mask = 0b0000;
            break;
        case INPUT_FLOATING:
            mode_mask = 0b0100;
            break;
        case INPUT_PULL_UP:
        case INPUT_PULL_DOWN:
            mode_mask = 0b1000;
            break;
        default:
            break;
    }// end switch mode

    /* Config the pin (CRH or CRL) */
    if(Pin > 7)  // Configuration Register High
    {
        GPIO->CRH &= ~(0b1111    << ((Pin-8)*4) );   // Reset pin to 0b0000
        GPIO->CRH |=  (mode_mask << ((Pin-8)*4) );   // Config the pin
    }
    else // Configuration Register Low
    {
        GPIO->CRL &= ~(0b1111    << (Pin*4) );   // Reset pin to 0b0000
        GPIO->CRL |=  (mode_mask << (Pin*4) );   // Config the pin
    }

    /* Config Input modes (Pull up or Pull down) */
    /* According to Table 20 on Reference Manual */
    if(mode == INPUT_PULL_UP)
    {
        GPIO->ODR |= (1<<Pin);
    }
    else if(mode == INPUT_PULL_DOWN)
    {
        GPIO->ODR &= ~(1<<Pin);
    }
}

/**
 * @brief Set or Reset a pin state.
 *
 * @param GPIO GPIOA~GPIOC
 * @param Pin 0~15
 * @param state 0 (reset) or 1 (set)
 */
void Gpio_Digital_Write(GPIO_TypeDef *GPIO, uint8_t Pin, _Bool state)
{
    if(state)   // set pin
    {
        GPIO->BSRR |= (1<<Pin);
    }
    else        // reset pin
    {
        GPIO->BRR |= (1<<Pin);
    }
}// end Gpio_Digital_Write


/**
 * @brief Toggle the state of a GPIO Pin.
 *
 * If pin state is Set, toggle to Reset. <br>
 * If pin state is Reset, toggle to Set.
 *
 * @param GPIO GPIOA~GPIOC
 * @param Pin 0~15
 */
void Gpio_Digital_Toggle(GPIO_TypeDef *GPIO, uint8_t Pin)
{
    GPIO->ODR ^= (1<<Pin);
}// end Gpio_Digital_Toggle


/**
 * @brief Get the state of a GPIO Pin
 *
 * @param GPIO GPIOA~GPIOC
 * @param Pin 0~15
 *
 * @retval _Bool 0 (Reset) or 1 (Set)
 */
_Bool Gpio_Digital_Read(GPIO_TypeDef *GPIO, uint8_t Pin)
{
    return (_Bool)( (GPIO->IDR & (1<<Pin)) >> Pin);
}
