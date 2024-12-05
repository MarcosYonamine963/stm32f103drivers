/**
 ******************************************************************************
 * @file      timer.h
 * @author    Marcos Yonamine
 * @version   v1.0
 * @date      2024-12-05
 *
 * @brief     Generic Timer management for STM32F103 devices
 *
 * ## Example of Usage
 *
 * ### 1. Edit the timer_list_t at timer.h with the list of used timers
 * User can add or remove timers on the list. But the last item must be TIMER_N_MAX.
 *
 *     typedef enum
 *     {
 *         TIMER_GENERIC,
 *
 *         TIMER_N_MAX
 *     }timer_list_t;
 *
 * ### 2. Create a function for the callback:
 *
 *     void myFunc(void);
 *
 * ### 3. At the main function, initialize the Timer module:
 *
 *     Timer_Init();
 *
 * ### 4. Set the Timer:
 * Ex: Call the function every 1 second <br>
 *
 *     Timer_Set(TIMER_GENERIC, TIME_1S, myFunc, TIMER_MODE_ALWAYS);
 *
 * ### 5. Execute the State Machine at embedded loop:
 *
 *     while(1)
 *     {
 *          Timer_SM();
 *     }
 *
 ******************************************************************************
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "stdint.h"

/*############################################*/

#define TIME_10MS   10
#define TIME_50MS   50
#define TIME_100MS  100
#define TIME_500MS  500
#define TIME_1S     1000
#define TIME_2S     2000
#define TIME_5S     5000

/**
 * @brief List of timers
 *
 * User can add or remove timer on the list. <br>
 * But the last item must be TIMER_N_MAX.
 */
typedef enum
{
    /* Begin User defined timers */
    TIMER_GENERIC,
    TIMER_BUTTON0_DEB,
    TIMER_BUTTON1_DEB,
    TIMER_BUTTON0_LONG_PRESS,
    TIMER_BUTTON1_LONG_PRESS,
    /* End User defined timers */

    /** @brief Must be the last item on the list.*/
    TIMER_N_MAX
}timer_list_t;

/*############################################*/

/** @brief Timer modes: use at Timer_Set function */
typedef enum
{
    TIMER_MODE_ONCE = 0,
    TIMER_MODE_ALWAYS
}timer_mode_e;

/** @brief Timer callback function */
typedef void (*Timer_CallbackFunc_t )(void);

void  Timer_Init(void);
void  Timer_Set(uint8_t timer_id, uint32_t time, Timer_CallbackFunc_t callback, timer_mode_e timer_mode);
_Bool Timer_Stop(uint8_t timer_id);
_Bool Timer_Continue(uint8_t timer_id);
_Bool Timer_Restart(uint8_t timer_id);
uint32_t Timer_Get_Counter(uint8_t timer_id);
_Bool Timer_Set_Counter(uint8_t timer_id, uint32_t counter);
void  Timer_SM(void);

uint32_t Timer_Get_Sys_Tick(void);
uint32_t Timer_Get_10us_Tick(void);

void  Delay_ms(uint32_t t);  // not recommended for using this function
void Delay_10us(uint32_t t); // not recommended for using this function

#ifdef  __cplusplus
}
#endif

#endif /* TIMER_H_ */
