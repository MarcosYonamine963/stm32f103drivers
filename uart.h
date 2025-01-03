#ifndef UART_H_
#define UART_H_

#include "stm32f1xx.h"

/*
 * FOR STM32F103C8T6:
 *
 * UART1 REMAP:
 *      NO_REMAP:       TX/PA9, RX/PA10
 *      REMAP:          TX/PB6, RX/PB7
 *
 * UART2 REMAP:
 *      NO_REMAP:       TX/PA2, RX/PA3
 *      REMAP           (UNSUPPORTED FOR STM32F103C8T6)
 *
 * UART3 REMAP:
 *      NO_REMAP:       TX/PB10, RX/PB11
 *      REMAP           (UNSUPPORTED FOR STM32F103C8T6)
 * */
typedef enum
{
    UART_NO_REMAP = 0,
    UART_REMAP,         // UNSUPPORTED FOR UART2
}uart_remap_e;

typedef enum
{
    UART_OK,
    UART_ERR
}uart_status_e;

// Function pointer for RX interrupt callback
typedef void (*Uart_RX_CallbackFunc_t )(uint8_t);

void Uart_config(USART_TypeDef *UARTx, uint32_t baud, uart_remap_e remap, Uart_RX_CallbackFunc_t callback);
uart_status_e Uart_WriteChar(USART_TypeDef *UARTx, uint8_t ch);
uart_status_e Uart_Transmit(USART_TypeDef *UARTx, uint8_t *buffer, uint16_t length);

#endif /* UART_H_ */
