#include "convert.h"


/**
 * @brief Converte um byte hexadecimal purto para dois bytes em formato Hex Ascii.
 *  EX: 0x3A -> "3A".
 *
 * @param byte Byte a ser convertido
 * @param result Ponteiro para a posicao inicial do buffer onde o valor
 * convertido deve ser salvo.
 */
void Convert_Byte_HAscii(uint8_t byte, uint8_t *result)
{
    uint8_t nibble;

    nibble = (uint8_t)(byte >> 4);
    nibble = (uint8_t)(nibble & 0x0F);

    if (nibble >= 0x0A)
    {
        *result = (uint8_t)(nibble + 0x37);
    }
    else
    {
        *result = (uint8_t)(nibble + 0x30);
    }

    nibble = (uint8_t)(byte & 0x0F);

    if (nibble >= 0x0A)
    {
        *(result + 1) = (uint8_t)(nibble + 0x37);
    }
    else
    {
        *(result + 1) = (uint8_t)(nibble + 0x30);
    }
}

/**
 * @brief Converte dois bytes formatados como hex ascii para um byte hexadecimal
 *  puro.
 *
 * @param buffer Ponteiro indiciando o inicio dos dois bytes for
 * @return uint8_t
 */
uint8_t Convert_HAscii_Byte(uint8_t *buffer)
{
    uint8_t value = 0;

    if (0x39 < buffer[0])
    {
        value = (uint8_t)(buffer[0] - 0x37);
    }
    else
    {
        value = (uint8_t)(buffer[0] - 0x30);
    }

    value <<= 4;

    if (0x39 < buffer[1])
    {
        value |= (uint8_t)(buffer[1] - 0x37);
    }
    else
    {
        value |= (uint8_t)(buffer[1] - 0x30);
    }

    return value;
}

/**
 * @brief Converte um inteiro sem sinal de 8 bits para uma string, no final
 * salva um '\0' para indicar o fim da string.
 *
 * @param num Numero a ser convertido.
 * @param result Ponteiro para a posicao inicial do buffer onde o valor
 * convertido deve ser salvo.
 */
void Convert_Uint8_Ascii(uint8_t num, uint8_t *result)
{
    result[3] = '\0';

    for (uint8_t i = 3; i > 0; i--)
    {
        result[i - 1] = (uint8_t)((num % 10) + 0x30);
        num /= 10;
    }
}

/**
 * @brief Converte um inteiro sem sinal de 16 bits para uma string, no final
 * salva um '\0' para indicar o fim da string.
 *
 * @param num Numero a ser convertido.
 * @param result Ponteiro para a posicao inicial do buffer onde o valor
 * convertido deve ser salvo.
 */
void Convert_Uint16_Ascii(uint16_t num, uint8_t *result)
{
    result[5] = '\0';

    for (uint8_t i = 5; i > 0; i--)
    {
        result[i - 1] = (uint8_t)((num % 10) + 0x30);
        num /= 10;
    }
}

/**
 * @brief Converte uma string que representa um numero em um inteiro sem sinal
 * de 8 bits.
 *
 * @param ascii Ponteiro para o inicio da string
 * @param len Tamanho da string
 * @return uint8_t resultado da conversao.
 */
uint8_t Convert_Ascii_Uint8(uint8_t * ascii, uint8_t len)
{
    uint8_t result = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        result *= 10;
        result += (uint8_t)(ascii[i] - 0x30);
    }

    return result;
}

/**
 * @brief Converte uma string que representa um numero em um inteiro sem sinal
 * de 16 bits.
 *
 * @param ascii Ponteiro para o inicio da string
 * @param len Tamanho da string
 * @return uint16_t resultado da conversao.
 */
uint16_t Convert_Ascii_Uint16(uint8_t *ascii, uint8_t len)
{
    uint16_t result = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        result *= 10;
        result += (uint8_t)(ascii[i] - 0x30);
    }

    return result;
}

/**
 * @brief Converte um inteiro sem sinal de 32 bits para uma string, no final
 * salva um '\0' para indicar o fim da string.
 *
 * @param num Numero a ser convertido.
 * @param result Ponteiro para a posicao inicial do buffer onde o valor
 * convertido deve ser salvo.
 */
void Convert_Uint32_Ascii(uint32_t num, uint8_t *result)
{
    result[10] = '\0';

    for (uint8_t i = 10; i > 0; i--)
    {
        result[i - 1] = (uint8_t)((num % 10) + 0x30);
        num /= 10;
    }
}

/**
 * @brief Converte uma string que representa um numero em um inteiro sem sinal
 * de 32 bits.
 *
 * @param ascii Ponteiro para o inicio da string
 * @param len Tamanho da string
 * @return uint16_t resultado da conversao.
 */
uint32_t Convert_Ascii_Uint32(uint8_t *ascii, uint8_t len)
{
    uint32_t result = 0;

    for (uint8_t i = 0; i < len; i++)
    {
        result *= 10;
        result += (uint8_t)(ascii[i] - 0x30);
    }

    return result;
}
