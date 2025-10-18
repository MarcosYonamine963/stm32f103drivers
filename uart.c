#include "uart.h"

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


/* PRIVATE FUNCTIONS PROTOTYPES */

static void Uart1_config(uint32_t baud, uart_remap_e remap);
static void Uart2_config(uint32_t baud);
static void Uart3_config(uint32_t baud);

// Function pointer for RX interrupt Callbacks
Uart_RX_CallbackFunc_t uart1_rx_callback;
Uart_RX_CallbackFunc_t uart2_rx_callback;
Uart_RX_CallbackFunc_t uart3_rx_callback;

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
void Uart_config(USART_TypeDef *UARTx, uint32_t baud, uart_remap_e remap, Uart_RX_CallbackFunc_t callback)
{
    switch((uint32_t)UARTx)
    {
        case (uint32_t)USART1:
            Uart1_config(baud, remap);
            uart1_rx_callback = callback;
            break;
        case (uint32_t)USART2:
            Uart2_config(baud);
            uart2_rx_callback = callback;
            break;
        case (uint32_t)USART3:
            Uart3_config(baud);
            uart3_rx_callback = callback;
            break;
    }
}

void Uart_change_baud(USART_TypeDef *UARTx, uint32_t baud)
{
    switch((uint32_t)UARTx)
    {
    	case (uint32_t)USART1:
			// Disable interrupts
    		__disable_irq();

			// Disable UART
			UARTx->CR1 &= ~(USART_CR1_UE);

			// Config Baud
			UARTx->BRR =  ((SystemCoreClock + (baud/2U))/baud);

			// Enable UART
			UARTx->CR1 |= USART_CR1_UE;

			// Enable interrupts
			__enable_irq();

		break;

    	case (uint32_t)USART2:

			// Disable interrupts
			__disable_irq();

			// Disable UART
			UARTx->CR1 &= ~(USART_CR1_UE);

			// Config Baud
			UARTx->BRR =  (((SystemCoreClock/2) + (baud/2U))/baud);

			// Enable UART
			UARTx->CR1 |= USART_CR1_UE;

			// Enable interrupts
			__enable_irq();

		break;

        case (uint32_t)USART3:

			// Disable interrupts
			__disable_irq();

			// Disable UART
			UARTx->CR1 &= ~(USART_CR1_UE);

			// Config Baud
			UARTx->BRR =  (((SystemCoreClock/2) + (baud/2U))/baud);

			// Enable UART
			UARTx->CR1 |= USART_CR1_UE;

			// Enable interrupts
			__enable_irq();

		break;
    }

}// end Uart_change_baud

void Uart_Disable(USART_TypeDef *UARTx)
{

    // Transmit Enable
	UARTx->CR1 &= ~USART_CR1_TE;

    // Receive Enable
	UARTx->CR1 &= ~USART_CR1_RE;

    // Enable Uart
	UARTx->CR1 &= ~USART_CR1_UE;

    // RXNE Interrupt Enable
	UARTx->CR1 &= ~USART_CR1_RXNEIE;
}

void Uart_Enable(USART_TypeDef *UARTx)
{
    // Transmit Enable
	UARTx->CR1 |= USART_CR1_TE;

    // Receive Enable
	UARTx->CR1 |= USART_CR1_RE;

    // Enable Uart
	UARTx->CR1 |= USART_CR1_UE;

    // RXNE Interrupt Enable
	UARTx->CR1 |= USART_CR1_RXNEIE;
}

uart_status_e Uart_Write_Byte(USART_TypeDef *UARTx, uint8_t data)
{
    // verify if uart is not enabled
    if(0 == (UARTx->CR1 & USART_CR1_UE))
        return UART_ERR;

    // verify if tx is not enabled
    if(0 == (UARTx->CR1 & USART_CR1_TE))
        return UART_ERR;

    /*Make sure the transmit data register is empty*/
    while(!(UARTx->SR & USART_SR_TXE)){}

    /*Write to transmit data register*/
    UARTx->DR  =  (data & 0xFF);

    return UART_OK;
}

uart_status_e Uart_Write_Array(USART_TypeDef *UARTx, uint8_t *array, uint16_t length)
{
    for(uint16_t i = 0; i < length; i++)
    {
        if( UART_ERR == Uart_Write_Byte(UARTx, array[i]))
            return UART_ERR;
    }

    return UART_OK;
}

uart_status_e Uart_Write_Text(USART_TypeDef *UARTx, char *text)
{
    if (!text)
        return UART_ERR;

    while (*text)
    {
        // verify if uart is not enabled
        if(0 == (UARTx->CR1 & USART_CR1_UE))
            return UART_ERR;

        // verify if tx is not enabled
        if(0 == (UARTx->CR1 & USART_CR1_TE))
            return UART_ERR;

        /*Make sure the transmit data register is empty*/
        while(!(UARTx->SR & USART_SR_TXE)){}

        /*Write to transmit data register*/
        UARTx->DR  =  (*text++ & 0xFF);
    }
    return UART_OK;
}


/* ####################################################### */

/* PRIVATE FUNCTIONS */

/*
 * remap:UART_NO_REMAP  : TX/PA9, RX/PA10
 * remap:UART_REMAP     : TX/PB6, RX/PB7
 */
static void Uart1_config(uint32_t baud, uart_remap_e remap)
{
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
    USART1->BRR =  ((SystemCoreClock + (baud/2U))/baud);

    // Enable Uart 1
    USART1->CR1 |= USART_CR1_UE;

    // RXNE Interrupt Enable
    USART1->CR1 |= USART_CR1_RXNEIE;

    // Enable USART Interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}

static void Uart2_config(uint32_t baud)
{
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
    USART2->BRR =  (((SystemCoreClock/2) + (baud/2U))/baud);

    // Enable Uart
    USART2->CR1 |= USART_CR1_UE;

    // RXNE Interrupt Enable
    USART2->CR1 |= USART_CR1_RXNEIE;

    // Enable USART Interrupt
    NVIC_EnableIRQ(USART2_IRQn);
}


static void Uart3_config(uint32_t baud)
{
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
    USART3->BRR =  (((SystemCoreClock/2) + (baud/2U))/baud);

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
    // RXNE: Received data ready to be read
    if( (USART1->SR & USART_SR_RXNE) == USART_SR_RXNE )
    {
        if(uart1_rx_callback)
        {
            uart1_rx_callback((uint8_t)USART1->DR);
        }
        else
        {
            (void)USART1->DR; // just read
        }
    }
}

void USART2_IRQHandler(void)
{
    // RXNE: Received data ready to be read
    if ((USART2->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
        if(uart2_rx_callback)
        {
            uart2_rx_callback((uint8_t)USART2->DR);
        }
        else
        {
            (void)USART2->DR; // just read
        }
    }
}

void USART3_IRQHandler(void)
{
    // RXNE: Received data ready to be read
    if ((USART3->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
        if(uart3_rx_callback)
        {
            uart3_rx_callback((uint8_t)USART3->DR);
        }
        else
        {
            (void)USART3->DR;
        }
    }
}
