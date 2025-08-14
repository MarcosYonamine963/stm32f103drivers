/**
 * @file circular_buffer.h
 *
 *
 */

#ifndef UTILS_CIRCULAR_BUFFER_H_
#define UTILS_CIRCULAR_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Sets the size of the circular buffer storage vector. Can be changed if
 *  a larger amount of space is required. Must be 2^N value. Watch out for the RAM consumption.
 *
 */
#define CIRCULAR_BUFFER_SIZE    0x400       //1024

/**
 * @brief Bit mask used to avoid overflow of indexes. Should be changed
 *  according to CIRCULAR_BUFFER_SIZE.
 *
 */
#define CIRCULAR_BUFFER_MASK    0x3FF

/**
 * @brief Struct definition of the Circular Buffer.
 *
 */
typedef struct FIFO_Circular_Buffer
{
    uint8_t data[CIRCULAR_BUFFER_SIZE]; /**< Data Array */
    uint16_t i_first;   /**< Index of the first position */
    uint16_t i_last;    /**< Index of the last position */
} circular_buffer_t ;

/**
 * @brief Result of operations performed on a circular buffer.
 *
 */
typedef enum STATUS_Circular_Buffer
{
    BUFFER_OK,
    BUFFER_EMPTY,
    BUFFER_FULL
} buffer_status_e;

void Circular_Buffer_Init(volatile circular_buffer_t * buffer);

uint8_t  Circular_Buffer_Is_Empty(volatile circular_buffer_t * buffer);
uint8_t  Circular_Buffer_Is_Full(volatile circular_buffer_t * buffer);
uint16_t Circular_Buffer_Used_Space(volatile circular_buffer_t * buffer);
uint16_t Circular_Buffer_Available_Space(volatile circular_buffer_t * buffer);

void Circular_Buffer_Flush(volatile circular_buffer_t * buffer);

buffer_status_e Circular_Buffer_Write_Byte(volatile circular_buffer_t * buffer, uint8_t byte);
buffer_status_e Circular_Buffer_Write_Array(volatile circular_buffer_t * buffer, const uint8_t * data, uint16_t length);

buffer_status_e Circular_Buffer_Read_Byte(volatile circular_buffer_t * buffer, uint8_t * byte);
uint16_t        Circular_Buffer_Read_Array(volatile circular_buffer_t * buffer, uint8_t * data, uint16_t max_length);

buffer_status_e Circular_Buffer_Peek_Byte(volatile circular_buffer_t * buffer, uint8_t * byte);
uint16_t        Circular_Buffer_Peek_Array(volatile circular_buffer_t * buffer, uint8_t * data, uint16_t max_length);

buffer_status_e Circular_Buffer_Peek_Last(volatile circular_buffer_t * buffer, uint8_t * byte);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_CIRCULAR_BUFFER_H_ */
