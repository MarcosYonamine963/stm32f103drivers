/**
 * @file i2c.c
 */

/*
    Reference:
    https://blog.embeddedexpert.io/?p=1493
*/

#include "i2c.h"
#include "timer.h"

// Timer for ensure the break of the infinite whiles. Use with Timer_Get_Sys_Tick() func.
static uint32_t i2c_timeout_timer;

/**
 * @brief Config pins for I2C1
 *
 *   Config I2C1 Remap:
 *
 *   I2C1_PIN_REMAP_NO:     <br>
 *       SCL: PB6           <br>
 *       SDA: PB7
 *
 *   I2C1_PIN_REMAP:        <br>
 *      SCL: PB8            <br>
 *      SDA: PB9
 *
 * @param I2C1_PIN_REMAPx see enum i2c1_pin_remap_e
 *
 */
i2c_status_e I2C1_Config(i2c1_pin_remap_e I2C1_PIN_REMAPx)
{
    // Enable GPIOB Clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    switch(I2C1_PIN_REMAPx)
    {
        case I2C1_PIN_REMAP_NO:
        default:
            /*
                SCL: PB6
                SDA: PB7
             */

            // Config I2C Remap
            AFIO->MAPR &= ~(AFIO_MAPR_I2C1_REMAP);

            // Config SCL (PB6) as Alternate Function Output Open Drain
            GPIOB->CRL |= (GPIO_CRL_MODE6); // Output 50 MHz
            GPIOB->CRL |= (GPIO_CRL_CNF6);  // AF Open Drain

            // Config SDA (PB7) as Alternate Function Output Open Drain
            GPIOB->CRL |= (GPIO_CRL_MODE7); // Output 50 MHz
            GPIOB->CRL |= (GPIO_CRL_CNF7);

            break;

        case I2C1_PIN_REMAP:
            /*
                SCL: PB8
                SDA: PB9
             */

            // Config I2C Remap
            AFIO->MAPR |= (AFIO_MAPR_I2C1_REMAP);

            // Config SCL (PB8) as Alternate Function Output Open Drain
            GPIOB->CRH |= (GPIO_CRH_MODE8); // Output 50 MHz
            GPIOB->CRH |= (GPIO_CRH_CNF8);  // AF Open Drain

            // Config SDA (PB9) as Alternate Function Output Open Drain
            GPIOB->CRH |= (GPIO_CRH_MODE8); // Output 50 MHz
            GPIOB->CRH |= (GPIO_CRH_CNF8);

            break;
    }// end switch remap

    /* ###   Config I2C1 (regardless of remap)   ### */

    // Enable AFIO Clock
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    // Enable I2C1 Clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Reset I2C1
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while(I2C1->CR1 & I2C_CR1_SWRST)
    {
        I2C1->CR1 &= ~I2C_CR1_SWRST;
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Frequency 32MHz
    I2C1->CR2 &= ~(I2C_CR2_FREQ); // Reset bits
    I2C1->CR2 |=  (32)<<(I2C_CR2_FREQ_Pos); // Set clock 32 MHz to I2C1

    I2C1->CCR |= (160)<<(I2C_CCR_FS_Pos); // Config to 100 kHz: (32 MHz / 2) / 100 kHz = 160

    // Maximum rise time in Fm/Sm mode (Master mode):
    I2C1->TRISE |= 33;  // (32 + 1) MHz

    // Enable I2C1
    I2C1->CR1 |= I2C_CR1_PE;

    return I2C_STATUS_OK;

}// end I2C1_Config

/**
 * @brief Scan bus from addr 0 to 127. Return quantity of found addrs.
 *
 * @param addr [out]    pointer to the list of addrs. Max len: 128.
 * @return              lenght of *addr (quant of addrs). <br>
 *                      if return == 0xFF: ERROR
 */
uint8_t I2C1_Scan_Bus(uint8_t *addr)
{
    uint8_t len = 0;

    // Ensure I2C1 is not busy
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while(I2C1->SR2 & I2C_SR2_BUSY)
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return 0xFF;
    }


    // scan the 128 addrs (7 bits: 0~127)
    for(uint8_t i = 0; i < 128; i++)
    {
        // Send Start Bit
        I2C1->CR1 |= I2C_CR1_START;

        // Wait for Start condition
        i2c_timeout_timer = Timer_Get_Sys_Tick();
        while( !(I2C1->SR1 & I2C_SR1_SB) )
        {
            if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
                return 0xFF;
        }

        // Send addr
        I2C1->DR=(i<<1);
        /* The reason behind the shifting is the address takes the bit1 to bit7
           from the data register and bit0 is used for read/write operation.
               bit0 = 0: write operation.
               bit0 = 1: read operation.
         */

        // Acho q ehh pra ler o registrador, e zerar as flags. TODO verificar isso
        i2c_timeout_timer = Timer_Get_Sys_Tick();
        while(!(I2C1->SR1)|!(I2C1->SR2))
        {
            if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
                return 0xFF;
        }

        // Send Stop bit
        I2C1->CR1 |= I2C_CR1_STOP;

        Delay_10us(10); // delay 100 us. TODO replace delay for other method (like verify the RX flag)
//        while(I2C1->SR2 & I2C_SR2_BUSY);
//        while(I2C1->SR1 & I2C_SR1_RXNE);

//        if( (I2C1->SR1 & I2C_SR1_ADDR) )
        if( 2 == (I2C1->SR1 & I2C_SR1_ADDR) ) // TODO verify this!
        {
            addr[len] = i;  // save the addr to the output vector
            len++;          // increment quant of addr found
        }

    }// end scan

    return len;
}

/**
 * @brief Write a byte (data) at the address (addr) of the slave (slave_addr).
 *
 * @param slave_addr [IN] address of the slave device (UID)
 * @param addr [IN] address of the slave to write the data (device memory addr)
 * @param data [IN] value to write
 *
 * @retval i2c_status_e Status of operation
 */
i2c_status_e I2C1_Write_Addr_Byte(uint8_t slave_addr, uint8_t addr, uint8_t data)
{
    // Ensure i2c is not busy
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while(I2C1->SR2 & I2C_SR2_BUSY)
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Send start
    I2C1->CR1 |= I2C_CR1_START;

    // Wait until start bit is set
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while( !(I2C1->SR1 & I2C_SR1_SB) )
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Send slave address
    I2C1->DR = slave_addr<< 1;

    // Wait until slave response for ADDR
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while( !(I2C1->SR1 & I2C_SR1_ADDR) )
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Clear SR2 by reading it
    (void)I2C1->SR2;

    // Wait until data register empty
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while( !(I2C1->SR1 & I2C_SR1_TXE) )
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Send addr to write
    I2C1->DR = addr;

    // Wait until data register empty
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while( !(I2C1->SR1 & I2C_SR1_TXE) )
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Send data to be written
    I2C1->DR = data;

    // Wait until data register empty
    i2c_timeout_timer = Timer_Get_Sys_Tick();
    while ( !(I2C1->SR1 & I2C_SR1_BTF) ) // BTF: Byte Transfer Fished status flag
    {
        if(Timer_Get_Sys_Tick() - i2c_timeout_timer > 500)
            return I2C_STATUS_ERR;
    }

    // Send Stop
    I2C1->CR1 |= I2C_CR1_STOP;

    return I2C_STATUS_OK;
}// end I2C1_Write_Addr_Byte
