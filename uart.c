#include "uart.h"
#include "sys_clock.h"
#include "circular_buffer.h"
#include "params.h"

/*
 * Transmission Procedure:
 * 1. Enable the USART by writing the UR bit in USART_CR1 register to 1.
 * 2. Program the M bit in USART_CR1 to define the word length.
 * 3. Program the number of stop bits in USART_CR2.
 * 4. Select DMA enable (DMAT) in USART_CR3 if Multi buffer Communication is to take place. Configure the DMA register as explained in multibuffer communication.
 * 5. Select the desired baud rate using the USART_BRR register.
 * 6. Set the TE bit in USART_CR1 to send an idle frame as first transmission
 * 7. Write the data to send in the USART_DR register (this clears the  TXE bit). Repeat this for each data to be transmitted in case of single buffer.
 * 8. After writing the last data into the USART_DR register, wait until TC=1. This indicates that the transmission of the last frame is complete. This is required for instance when the USART is disabled or enters the Halt mode to aviod corrupting the last transmission.
 *
 * */

/*
 * Reception Procedure:
 * 1. Enable the USART by writing the UE bit in USART_CR1 register to 1.
 * 2. Program the M bit in USART_CR1 to define the word length.
 * 3. Program the number of stop bits in USART_CR2.
 * 4. Select DMA enable (DMAR) in USART_CR3 if multibuffer communication is to take place. Configure the DMA register as explained in multibuffer communication. STEP 3
 * 5. Select the desired baud rate using the baud rate register USART_BRR
 * 6. Set the RE bit USART_CR1. This enables the receiver which begins searching for a start bit.
 *
 * When a character is received:
 * The EXNE bit is set. It indicates that the content of the shift register is transferred to the RDR. In other words, data has been received and cam be read (as well as its associated error flags).
 * An interrupt is generated if the RXNEIE bit is set.
 * The error flags can be set if a frame error, noise or an overrun error has been detected during reception.
 * In multibuffer, RXNE is set after every byte received and is cleared by the DMA read to the Data register.
 * In single buffer mode, clearing the RXNE bit is performed by a software read to the USART_DR register. The RXNE flag can also be cleared by writing a zero to it. The RXNE bit must be cleared before the end of the reception of the next character to avoid an overrun error.
 *
 * Note: The RE bit should not be reset while receiving data. If the RE bit is disabled during reception, the reception of the current byte will be aborted.
 *
 * */

/* ####################################################### */


/* PRIVATE VARIABLES */

static circ_buffer_t uart1_buffer = {{0}, 0, 0};
static circ_buffer_t uart2_buffer = {{0}, 0, 0};
static circ_buffer_t uart3_buffer = {{0}, 0, 0};


/* ####################################################### */


/* PRIVATE FUNCTIONS PROTOTYPES */

static void Uart1_config(uint32_t baud, uart_remap_e remap);
static void Uart2_config(uint32_t baud);
static void Uart3_config(uint32_t baud);

/* ####################################################### */

/* EXPORTED FUNCTIONS */

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
void Uart_config(USART_TypeDef *UARTx, uint32_t baud, uart_remap_e remap)
{
    switch((uint32_t)UARTx)
    {
        case (uint32_t)USART1:
            Uart1_config(baud, remap);
            break;
        case (uint32_t)USART2:
            Uart2_config(baud);
            break;
        case (uint32_t)USART3:
            Uart3_config(baud);
            break;
    }
}


void Uart_WriteChar(USART_TypeDef *UARTx, uint8_t ch)
{
    /*Make sure the transmit data register is empty*/
    while(!(UARTx->SR & USART_SR_TXE)){}

    /*Write to transmit data register*/
    UARTx->DR  =  (ch & 0xFF);
}

void Uart_Transmit(USART_TypeDef *UARTx, uint8_t *buffer, uint16_t length)
{
    for(uint16_t i = 0; i < length; i++)
    {
        Uart_WriteChar(UARTx, buffer[i]);
    }
}

/* @brief Read a received byte
 * @param UARTx: USART1, USART2 or USART3
 * @param read_value: pointer to read data
 *
 * @ret UART_OK or UART_ERR
 * */
uart_status_e Uart_Read_from_buffer(USART_TypeDef *UARTx, uint8_t *read_value)
{
    switch((uint32_t)UARTx)
    {
        case (uint32_t)USART1:
            if( Buffer_Read(&uart1_buffer, read_value) == BUFFER_OK)
            {
                return UART_OK;
            }
            else
            {
                return UART_ERR;
            }
            break;

        case (uint32_t)USART2:

            if( Buffer_Read(&uart2_buffer, read_value) == BUFFER_OK)
            {
                return UART_OK;
            }
            else
            {
                return UART_ERR;
            }
            break;

        case (uint32_t)USART3:

            if( Buffer_Read(&uart3_buffer, read_value) == BUFFER_OK)
            {
                return UART_OK;
            }
            else
            {
                return UART_ERR;
            }
            break;

        default:
            return UART_ERR;
    }
}// end Uart_Read_from_buffer


/* ####################################################### */

/* PRIVATE FUNCTIONS */

/*
 * remap:UART_NO_REMAP  : TX/PA9, RX/PA10
 * remap:UART_REMAP     : TX/PB6, RX/PB7
 */
static void Uart1_config(uint32_t baud, uart_remap_e remap)
{
    // Tx config done.  TODO Rx config.

    if(UART_REMAP == remap)
    {
        /* TX/PB6, RX/PB7 */

        // Enable clock access to GPIOB
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        // Enable clock access to alternate function
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
        // Enable clock access to USART1
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

        // PB6 as output max speed 50MHz
        GPIOB->CRL |= GPIO_CRL_MODE6;

        // PB6 as AF Push-Pull
        GPIOB->CRL |=  GPIO_CRL_CNF6_1;
        GPIOB->CRL &= ~GPIO_CRL_CNF6_0;

        // Remap
        AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
    }
    else if (UART_NO_REMAP == remap)
    {
        /* TX/PA9, RX/PA10 */

        // Enable clock access to GPIOA
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        // Enable clock access to alternate function
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
        // Enable clock access to USART1
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

        // PA9 as output max speed 50MHz
        GPIOA->CRH |= GPIO_CRH_MODE9;

        // PA9 as AF Push-Pull
        GPIOA->CRH |=  GPIO_CRH_CNF9_1;
        GPIOA->CRH &= ~GPIO_CRH_CNF9_0;

        // Dont Remap
        AFIO->MAPR&=~AFIO_MAPR_USART1_REMAP;
    }


    // Transmit Enable
    USART1->CR1 |= USART_CR1_TE;

    // Receive Enable
    USART1->CR1 |= USART_CR1_RE;

    // Config Mantissa and Fraction
    USART1->BRR =  ((SYS_CLOCK + (baud/2U))/baud);

    // Enable Uart 1
    USART1->CR1 |= USART_CR1_UE;

    // RXNE Interrupt Enable
    USART1->CR1 |= USART_CR1_RXNEIE;

    // Enable USART Interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}

static void Uart2_config(uint32_t baud)
{
    // Tx config done.  TODO Rx config.

    /* TX/PA2, RX/PA3 */

    // Enable clock access to GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // Enable clock access to alternate function
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // Enable clock access to USART2
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // PA2 as output max speed 50MHz
    GPIOA->CRL |= GPIO_CRL_MODE2;

    // PA9 as AF Push-Pull
    GPIOA->CRL |=  GPIO_CRL_CNF2;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;

    // Dont Remap
    AFIO->MAPR&=~AFIO_MAPR_USART2_REMAP;


    //Transmit Enable
    USART2->CR1 |= USART_CR1_TE;

    // Receive Enable
    USART2->CR1 |= USART_CR1_RE;

    // Config Mantissa and Fraction
    USART2->BRR =  (((SYS_CLOCK/2) + (baud/2U))/baud);

    // Enable Uart
    USART2->CR1 |= USART_CR1_UE;

    // RXNE Interrupt Enable
    USART2->CR1 |= USART_CR1_RXNEIE;

    // Enable USART Interrupt
    NVIC_EnableIRQ(USART2_IRQn);
}


static void Uart3_config(uint32_t baud)
{
    // Tx config done.  TODO Rx config.

    /* TX/PB10, RX/PB11 */

    // Enable clock access to GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    // Enable clock access to alternate function
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // Enable clock access to USART
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // PB10 as output max speed 50MHz
    GPIOB->CRH |= GPIO_CRH_MODE10;

    // PA9 as AF Push-Pull
    GPIOB->CRH |=  GPIO_CRH_CNF10_1;
    GPIOB->CRH &= ~GPIO_CRH_CNF10_0;

    // Dont Remap
    AFIO->MAPR&=~AFIO_MAPR_USART3_REMAP;



    //Transmit Enable
    USART3->CR1 |= USART_CR1_TE;

    // Receive Enable
    USART3->CR1 |= USART_CR1_RE;

    // Config Mantissa and Fraction
    USART3->BRR =  (((SYS_CLOCK/2) + (baud/2U))/baud);

    // Enable Uart
    USART3->CR1 |= USART_CR1_UE;

    // RXNE Interrupt Enable
    USART3->CR1 |= USART_CR1_RXNEIE;

    // Enable USART Interrupt
    NVIC_EnableIRQ(USART3_IRQn);
}

/* ####################################################### */

/* INTERRUPT HANDLERS */

void USART1_IRQHandler(void)
{
    uint8_t usart_byte_in;

    // RXNE: Received data ready to be read
    if( (USART1->SR & USART_SR_RXNE) == USART_SR_RXNE )
    {
        usart_byte_in = (uint8_t)USART1->DR;
        Buffer_Write(&uart1_buffer, usart_byte_in);
    }
}

void USART2_IRQHandler(void)
{
    uint8_t usart_byte_in;

    // RXNE: Received data ready to be read
    if ((USART2->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
        usart_byte_in = (uint8_t)USART2->DR;
        Buffer_Write(&uart2_buffer, usart_byte_in);
    }
}

void USART3_IRQHandler(void)
{
    uint8_t usart_byte_in;

    // RXNE: Received data ready to be read
    if ((USART3->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
        usart_byte_in = (uint8_t)USART3->DR;
        Buffer_Write(&uart3_buffer, usart_byte_in);
    }
}

void debug_send_msg(uint8_t *msg, uint8_t size)
{
    if(params.debug_cfg & (1<<7))
        Uart_Transmit(UART_DEBUG, msg, size);
}// end debug_send_msg