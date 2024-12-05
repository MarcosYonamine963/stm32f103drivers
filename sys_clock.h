/**
 ******************************************************************************
 * @file      sys_clock.h
 * @author    Marcos Yonamine
 * @version   v1.0
 * @date      2024-12-05
 *
 * @brief     System Clock configuration file for STM32F103 devices
 *
 ******************************************************************************
 */

#ifndef SYS_CLOCK_H_
#define SYS_CLOCK_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* Clock values */

#define CLOCK_64MHz     64000000UL
#define CLOCK_32MHz     32000000UL
#define CLOCK_16MHz     16000000UL
#define CLOCK_8MHz       8000000UL

/** @brief Select System Clock to initialize */
#define SYS_CLOCK       CLOCK_64MHz

void Sys_Clock_Init(void);

#ifdef  __cplusplus
}
#endif

#endif /* SYS_CLOCK_H_ */
