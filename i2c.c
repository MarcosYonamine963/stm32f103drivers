#include "i2c.h"

// ##############  PRIVATE FUNCTIONS  ############### //

/**
 * I2C1 No Remap:
 * SCL: PB6
 * SDA: PB7
 *
 * I2C1 Remap:
 * SCL: PB8
 * SDA: PB9
 */
static void _I2C1_Config(i2c_remap_e I2C_REMAP_x, i2c_freq_e I2C_FREQ_x)
{
    // Enable GPIOB clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    // Enable AFIO clock
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // Enable I2C1 clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    switch(I2C_REMAP_x)
    {
        case I2C_REMAP_NO_REMAP:
            // PB6: AF Open-Drain
            GPIOB->CRL |= GPIO_CRL_MODE6 | GPIO_CRL_CNF6;
            // PB7: AF Open-Drain
            GPIOB->CRL |= GPIO_CRL_MODE7 | GPIO_CRL_CNF7;
            // No remap
            AFIO->MAPR &= ~(AFIO_MAPR_I2C1_REMAP);
            break;

        case I2C_REMAP_REMAP:
            // PB8: AF Open-Drain
            GPIOB->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_CNF8;
            // PB9: AF Open-Drain
            GPIOB->CRH |= GPIO_CRH_MODE9 | GPIO_CRH_CNF9;
            // Remap
            AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
            break;

        default:
            break;

    }// end switch remap

    // Program I2C input clock at I2C_CR2
    I2C1->CR2 |= 32; // 32 MHz: APB1 clock frequency value. (maximum 50 MHz)

    // Configure the clock control registers
    // Standard mode
    I2C1->CCR &= ~(I2C_CCR_FS);

    // Config frequency
    switch(I2C_FREQ_x)
    {
        case I2C_FREQ_20K:
            I2C1->CCR |= (800 << I2C_CCR_CCR_Pos); // CCR = PCLK1 / (2 * Target I2C Frequency). Calculation for 20 kHz
            break;
        case I2C_FREQ_50K:
            I2C1->CCR |= (320 << I2C_CCR_CCR_Pos);
            break;
        case I2C_FREQ_100K:
            I2C1->CCR |= (160 << I2C_CCR_CCR_Pos);
            break;
    }

    // Configure the rise time register
    // Max value: PCLK1 + 1. Register max value: 31
    I2C1->TRISE |= (31);

    // Program the I2C_CR1 register to enable the peripheral
    I2C1->CR1 |= I2C_CR1_PE;

}// end _I2C1_Config

/**
 * I2C2 No Remap:
 * SCL: PB10
 * SDA: PB11
 *
 * I2C2 Remap not available.
 */
static void _I2C2_Config(i2c_freq_e I2C_FREQ_x)
{
    // Enable GPIOB clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    // Enable AFIO clock
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // Enable I2C2 clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

    // PB10: AF Open-Drain
    GPIOB->CRH |= GPIO_CRH_MODE10 | GPIO_CRH_CNF10;
    // PB11: AF Open-Drain
    GPIOB->CRH |= GPIO_CRH_MODE11 | GPIO_CRH_CNF11;

    // Program I2C input clock at I2C_CR2
    I2C2->CR2 |= 32; // 32 MHz: APB1 clock frequency value. (maximum 50 MHz)

    // Configure the clock control registers
    // Standard mode
    I2C2->CCR &= ~(I2C_CCR_FS);

    // Config frequency
    switch(I2C_FREQ_x)
    {
        case I2C_FREQ_20K:
            I2C2->CCR |= (800 << I2C_CCR_CCR_Pos); // CCR = PCLK1 / (2 * Target I2C Frequency). Calculation for 20 kHz
            break;
        case I2C_FREQ_50K:
            I2C2->CCR |= (320 << I2C_CCR_CCR_Pos);
            break;
        case I2C_FREQ_100K:
            I2C2->CCR |= (160 << I2C_CCR_CCR_Pos);
            break;
    }

    // Configure the rise time register
    // Max value: PCLK1 + 1. Register max value: 31
    I2C2->TRISE |= (31);

    // Program the I2C_CR1 register to enable the peripheral
    I2C2->CR1 |= I2C_CR1_PE;
}

// ##############  PUBLIC FUNCTIONS  ############### //

void I2C_Config(I2C_TypeDef *I2Cx, i2c_remap_e I2C_REMAP_x, i2c_freq_e I2C_FREQ_x)
{
    switch((uint32_t)(I2Cx))
    {
        case (uint32_t)I2C1:
            _I2C1_Config(I2C_REMAP_x, I2C_FREQ_x);
            break;

        case (uint32_t)I2C2:
            _I2C2_Config(I2C_FREQ_x);
            break;

        default:
            break;
    }
}


i2c_status_e I2C_is_Busy(I2C_TypeDef *I2Cx)
{
    if(I2Cx->SR2 & I2C_SR2_BUSY)
        return I2C_STATUS_BUSY;
    else
        return I2C_STATUS_OK;
}

i2c_status_e I2C_Send_Start(I2C_TypeDef *I2Cx, uint8_t slave_addr, i2c_data_dir_e I2C_DATA_DIR_x)
{
    if(I2C_DATA_DIR_x == I2C_DATA_DIR_WRITE)
    {
        slave_addr &= ~(0b1);
    }
    else
    {
        slave_addr |= (0b1);
    }

    // Start Condition
    I2Cx->CR1 |= I2C_CR1_START;

    // Read SR1
    while( !(I2Cx->SR1 & I2C_SR1_SB));

    // Write DR with the slave address
    I2Cx->DR = slave_addr;

    // Read SR1
    while(!(I2Cx->SR1 & I2C_SR1_ADDR));
    while( (I2Cx->SR1 & I2C_SR1_ADDR) )
    {
        I2Cx->SR1;
        I2Cx->SR2;
        if( !(I2Cx->SR1 & I2C_SR1_ADDR) )
        {
            break;
        }
    }

    return I2C_STATUS_OK;
}// end I2C_Send_Start

i2c_status_e I2C_Write_Data(I2C_TypeDef *I2Cx, uint8_t data)
{
    while(!(I2Cx->SR1 & I2C_SR1_TXE));
    I2Cx->DR = data;
    while(!(I2Cx->SR1 & I2C_SR1_TXE));

    return I2C_STATUS_OK;
}

i2c_status_e I2C_Read_Data(I2C_TypeDef *I2Cx, uint8_t *data)
{
    while(!(I2Cx->SR1 & I2C_SR1_RXNE));
    uint8_t read = (uint8_t)(I2Cx->DR);
    *data = read;

    return I2C_STATUS_OK;
}

i2c_status_e I2C_Send_Stop(I2C_TypeDef *I2Cx)
{
    I2Cx->SR1;
    I2Cx->SR2;

    while(!(I2Cx->SR1 & I2C_SR1_BTF));

    // Generate STOP condition
    I2Cx->CR1 |= I2C_CR1_STOP;

    // Espera ate ficar IDLE:
    while(I2Cx->SR2 & I2C_SR2_BUSY);

    return I2C_STATUS_OK;
}

void I2C_Clear_Received_ACK_flag(I2C_TypeDef *I2Cx)
{
    I2Cx->CR1 &= ~(I2C_CR1_ACK);
}
