/**
 ******************************************************************************
 * @file      circular_buffer.h
 * @author    Marcos Yonamine
 * @version   v1.0
 * @date      2024-12-05
 *
 * @brief     Circular FIFO Buffer.
 *
 * ## Example of use
 *
 * ### 1. Create a circular buffer variable
 *
 *     circ_buffer_t buffer = { {0}, 0, 0 };
 *
 * ### 2. Write a byte on the buffer
 *
 *     Buffer_Write(&buffer, value);
 *
 * ### 3. Read the byte, and save to a variable
 *
 *     if(BUFFER_OK == Buffer_Read(&buffer, &var))
 *     {
 *         do_something(var);
 *     }
 *
 ******************************************************************************
 */

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @brief Sets the size of the circular buffer storage vector, can be changed if 
 *  a larger amount of space is required. Watch out for the RAM consumption.
 * 
 */
#define SIZE_CIRCULAR_BUFFER    0x400       //1024

/**
 * @brief Bit mask used to avoid overflow of indexes should be changed
 *  according to SIZE_CIRCULAR_BUFFER.
 * 
 */
#define CIRCULAR_BUFFER_MASK    (SIZE_CIRCULAR_BUFFER - 1)

/**
 * @brief Struct definition of the Circular buffer.
 * 
 */
typedef struct FIFO_Circular_Buffer
{

    /** @brief Dara Array */
	uint8_t data[SIZE_CIRCULAR_BUFFER]; /**< .*/

    /** @brief Index of the first position */
	uint16_t i_first; /**< .*/

	/** @brief Index of the last position */
	uint16_t i_last; /**< .*/

} circ_buffer_t ;

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

buffer_status_e Buffer_Write(volatile circ_buffer_t * buffer, uint8_t byte);
buffer_status_e Buffer_Read(volatile circ_buffer_t * buffer, uint8_t * byte);
buffer_status_e Buffer_Peek(volatile circ_buffer_t * buffer, uint8_t * byte);


#ifdef  __cplusplus
}
#endif

#endif
