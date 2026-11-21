/*
 * core_delay.h
 *
 *  Created on: Nov 17, 2026
 *      Author: Marcos Yonamine
 */

#ifndef STM32F103DRIVERS_CORE_DELAY_H_
#define STM32F103DRIVERS_CORE_DELAY_H_

#include <stdint.h>
#include "stm32f1xx.h"

static inline void core_timer_init(void) {
    // Verifica diretamente no hardware se o contador já está habilitado e rodando
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0U) {
        return;
    }
    SystemCoreClockUpdate(); // Garante que SystemCoreClock reflete o RCC atual
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    #if defined(DWT_LAR_KEY_Msk) || (defined(__CORTEX_M) && (__CORTEX_M == 7U))
    DWT->LAR = 0xC5ACCE55;
    #endif

    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t core_get_cycles(void) {
    return DWT->CYCCNT;
}

static inline uint32_t core_us_to_cycles(uint32_t us) {
    // Divide antes para evitar overflow na multiplicação em clocks altos
    return us * (SystemCoreClock / 1000000U);
}

static inline void core_delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t target = core_us_to_cycles(us);
    while ((DWT->CYCCNT - start) < target) {
        __NOP();
    }
}

#endif /* STM32F103DRIVERS_CORE_DELAY_H_ */
