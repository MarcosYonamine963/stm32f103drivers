#include "stm32f1xx.h"
#include "timer.h"

#ifndef NULL
#define NULL ((void *)0x00)
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef TMR_AMOUNT
#define TMR_AMOUNT    20
#endif

typedef struct timer
{
    timer_callback_t cbk;
    timer_type_t type;
    uint32_t period;
    uint32_t timeout;
    timer_state_t state;
}
timer_t;

static timer_t timers[TMR_AMOUNT] = {{NULL, TYPE_NOT_DEFINED, 0, 0, TIMER_EMPTY}};
static volatile uint32_t systemtick = 0;
static volatile uint32_t delay_time = 0;
static uint8_t timer_is_initialized = 0;

/**
 * @brief Configure System Clock: HSI and PLL. 64MHz as main system Clock.
 *
 */
static void Sys_Clock_Init(void)
{
    /* 1. Enable HSI and wait for HSI to become ready */
    RCC->CR |= RCC_CR_HSION;
    while( !(RCC->CR & RCC_CR_HSIRDY) );// when done, HSERDY is set

    /* 2. Set the Power Enable bit */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* 3. Configure the FLASH PREFETCH and the LATENCY Related Settings */
    /*
       FLASH_ACR_LATENCY_0 if 0 < SYSCLK <= 24 MHz
       FLASH_ACR_LATENCY_1 if 24 MHz < SYSCLK <= 48 MHz
       FLASH_ACR_LATENCY_2 if 48 MHz < SYSCLK <= 72 MHz
     */
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    /* 4. Configure the PRESCALERS HCLK, PCLK1, PCLK2 */
    // AHB Prescaler
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    // APB1 Prescaler
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    // APB2 Prescaler
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    /* 5. Configure the MAIN PLL */
    RCC->CFGR |= RCC_CFGR_PLLMULL16;             // Multiply PLL internal clock: (HSI/2)*PLLMULL

    /* 6. Enable PLL and wait for PLL to become ready */
    RCC->CR |= RCC_CR_PLLON;
    while( !(RCC->CR & RCC_CR_PLLRDY) );

    /* 7. Select the System Clock Source and wait to become ready */
    RCC->CFGR |= RCC_CFGR_SW_PLL;               // Select PLL as System Clock
    while( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL );
}


void SysTick_Handler(void)
{
	systemtick++;
	delay_time++;
}


/**
 * @brief Initialization of the timers. Config the SysTick interrupt and
 * reset all the timers flags. Execute the function Sys_Clock_Init before this.
 */
void Timer_Init(void)
{
	Sys_Clock_Init();

	SystemCoreClockUpdate();

    /* https://www.keil.com/pack/doc/CMSIS_Dev/Core/html/group__system__init__gr.html */
    /* Config SysTick interrupt. Callback: SysTick_Handler */
	SysTick_Config(SystemCoreClock / TIMEBASE);

	timer_is_initialized = 1;

    for (uint16_t i = 0; i < TMR_AMOUNT; i++)
    {
        timers[i].state = TIMER_EMPTY;
        timers[i].type = TYPE_NOT_DEFINED;
    }

	NVIC_SetPriority(SysTick_IRQn, 1);

}// end Timer_Init



/**
 * @brief Get current System tick value.
 *
 * @return uint32_t System tick.
 */
uint32_t Timer_GetSystemTick(void)
{
	return systemtick;
}

void Timer_SM(void)
{
    static uint16_t timer_cnt = 0;
    uint32_t tick = systemtick;

    if (0 == timer_is_initialized)
    {
        return;
    }

    if ((TIMER_RUNNING == timers[timer_cnt].state) && \
        (systemtick >= timers[timer_cnt].timeout))
    {
        if (NULL != timers[timer_cnt].cbk)
        {
            timers[timer_cnt].cbk((void*)&tick, 1);
        }
        else
        {
            timers[timer_cnt].state = TIMER_ERROR;
        }

        switch (timers[timer_cnt].type)
        {
            case ONE_SHOT_TIMER:
                timers[timer_cnt].state = TIMER_STOPPED;
            break;

            case AUTO_RELOAD_TIMER:
                timers[timer_cnt].timeout += timers[timer_cnt].period;
            break;

            default:
                timers[timer_cnt].state = TIMER_ERROR;
            break;
        }

    }

    if (TMR_AMOUNT <= (++timer_cnt))
    {
        timer_cnt = 0;
    }

}

uint16_t Timer_Create(timer_callback_t timer_cbk, timer_type_t timer_type, \
    uint32_t timer_period)
{
    uint16_t result = UINT16_MAX;

    for (uint16_t i = 0; i < TMR_AMOUNT; i++)
    {
        if (TIMER_EMPTY == timers[i].state)
        {
            if (NULL == timer_cbk)
            {
                /* do nothing */
            }
            else if ((AUTO_RELOAD_TIMER < timer_type) || (0 == timer_period))
            {
                /* do nothing */
            }
            else
            {
                timers[i].cbk = timer_cbk;
                timers[i].type = timer_type;
                timers[i].period = timer_period;
                timers[i].timeout = (uint32_t)(systemtick + timer_period);
                timers[i].state = TIMER_RUNNING;

                result = i;
            }

            break;
        }
        else
        {
            /* do nothing */
        }

    }

    return result;
}

void Timer_Config(uint16_t timer_id, timer_callback_t timer_cbk, \
    timer_type_t timer_type, uint32_t timer_period)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else if (TIMER_EMPTY != timers[timer_id].state)
    {
        if (NULL != timer_cbk)
        {
            timers[timer_id].cbk = timer_cbk;
        }
        else
        {
            /* do nothing */
        }


        if (KEEP_TIMER_TYPE > timer_type)
        {
            timers[timer_id].type = timer_type;
        }
        else
        {
            /* do nothing */
        }

        if (0 != timer_period)
        {
            timers[timer_id].period = timer_period;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }
}

void Timer_Delete(uint16_t timer_id)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else
    {
        timers[timer_id].state = TIMER_EMPTY;
    }

}

void Timer_Start(uint16_t timer_id)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else if (TIMER_EMPTY != timers[timer_id].state)
    {
        timers[timer_id].timeout = (uint32_t)(systemtick + timers[timer_id].period);
        timers[timer_id].state = TIMER_RUNNING;
    }
    else
    {
        /* do nothing */
    }
}

/**
 * @brief Stops a running timer.
 *
 * @param timer_id Timer ID.
 */
void Timer_Stop(uint16_t timer_id)
{
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else if (TIMER_EMPTY != timers[timer_id].state)
    {
        timers[timer_id].state = TIMER_STOPPED;
    }
    else
    {
        /* do nothing */
    }
}

/**
 * @brief Get timer current state.
 *
 * @param timer_id Timer ID.
 * @return timer_state_t Timer state.
 */
timer_state_t Timer_GetTimerState(uint16_t timer_id)
{
    timer_state_t state = TIMER_ERROR;
    if (TMR_AMOUNT <= timer_id)
    {
        /* do nothing */
    }
    else
    {
        state = timers[timer_id].state;
    }

    return state;
}

/**
 * @brief Delay in milisseconds
 *
 * @param time_ms time to delay (ms)
 */
void Timer_Delay(uint32_t time_ms)
{
    delay_time = 0;
    while (delay_time < time_ms*TIME_1MS)
    {
        asm volatile("nop");
    }
}

void Timer_Delay_10us(uint32_t time_10us)
{
    delay_time = 0;
    while (delay_time < time_10us*2)
    {
        asm volatile("nop");
    }
}

void Timer_Delay_5us(uint32_t time_5us)
{

	delay_time = 0;

    while (delay_time < time_5us)
    {
        asm volatile("nop");
    }

}


