/**
 * @file circular_buffer.c
 *
 *
 */

#include "circular_buffer.h"

/**
 * @brief Init variables of the circular buffer.
 *
 * @param buffer [IN]: Circular buffer to be initialized.
 */
void Circular_Buffer_Init(volatile circular_buffer_t * buffer)
{
    buffer->i_first = 0;
    buffer->i_last = 0;
}

/**
 * @brief Verify if the circular buffer is empty.
 *
 * @param buffer [IN]: Circular buffer to be analyzed.
 *
 * @retval 1 if empty, 0 if not empty.
 */
uint8_t Circular_Buffer_Is_Empty(volatile circular_buffer_t * buffer)
{
    return (buffer->i_first == buffer->i_last);
}

/**
 * @brief Verify if the circular buffer is full.
 *
 * @param buffer [IN]: Circular buffer to be analyzed.
 *
 * @retval 1 if full, 0 if not full.
 */
uint8_t Circular_Buffer_Is_Full(volatile circular_buffer_t * buffer)
{
    return (((buffer->i_last + 1) & CIRCULAR_BUFFER_MASK) == buffer->i_first);
}

/**
 * @brief Get amount of bytes saved on the buffer.
 *
 * @param buffer [IN]: Circular buffer to be analyzed.
 *
 * @retval uint16_t quantity of bytes saved on the buffer.
 */
uint16_t Circular_Buffer_Used_Space(volatile circular_buffer_t * buffer)
{
    return (buffer->i_last - buffer->i_first) & CIRCULAR_BUFFER_MASK;
}

/**
 * @brief Get the available space (bytes) in the buffer (i.e., how many bytes can still be written).
 *
 * @param buffer [IN]: Circular buffer to be analyzed.
 *
 * @retval uint16_t number of bytes available to write.
 */
uint16_t Circular_Buffer_Available_Space(volatile circular_buffer_t * buffer)
{
    return (CIRCULAR_BUFFER_SIZE - Circular_Buffer_Used_Space(buffer) - 1);
}

/**
 * @brief Clear the buffer.
 *
 * @param buffer [IN]: buffer to be cleared.
 */
void Circular_Buffer_Flush(volatile circular_buffer_t * buffer)
{
    buffer->i_first = buffer->i_last;
}

/**
 * @brief Writes a new data byte on a circular buffer.
 *
 * @param buffer [IN]: Circular buffer to receive the new data.
 * @param byte [IN]: Data to be written.
 * @retval buffer_status_e: Operation status, returns if the buffer is
 *  full or if the data could be saved.
 */
buffer_status_e Circular_Buffer_Write_Byte(volatile circular_buffer_t * buffer, uint8_t byte)
{
    uint16_t next = (buffer->i_last + 1) & CIRCULAR_BUFFER_MASK;

    if (next == buffer->i_first)
        return BUFFER_FULL;

    buffer->data[buffer->i_last] = byte;
    buffer->i_last = next;

    return BUFFER_OK;
}

/**
 * @brief Writes multiple bytes on the buffer.
 *
 * All-or-nothing function: if length is greater than available space, do not write (return BUFFER_FULL).
 *
 * @param buffer [IN]: buffer to be written to.
 * @param data [IN]: data array to be written on the buffer.
 * @param length [IN]: length (quantity in bytes) of the data array.
 * @retval buffer_status_e: Operation status, returns if the buffer is
 * full or if the data could be saved.
 *
 * If length is greater than available space,
 */
buffer_status_e Circular_Buffer_Write_Array(volatile circular_buffer_t * buffer, const uint8_t * data, uint16_t length)
{
    if(Circular_Buffer_Available_Space(buffer) < length)
        return BUFFER_FULL;

    for (uint16_t i = 0; i < length; i++)
    {
        Circular_Buffer_Write_Byte(buffer, data[i]);
    }

    return BUFFER_OK;
}

/**
 * @brief Reads the oldest byte saved on a circular buffer. The byte in the buffer is removed.
 *
 * @param buffer [IN]: Circular buffer from where data will be read.
 * @param byte {OUT}: Pointer to the byte that must received the read data.
 * @retval buffer_status_e: Operation status, returns if the buffer is empty or
 * if the data could be read.
 */
buffer_status_e Circular_Buffer_Read_Byte(volatile circular_buffer_t * buffer, uint8_t * byte)
{
    if (buffer->i_first == buffer->i_last)
        return BUFFER_EMPTY;

    *byte = buffer->data[buffer->i_first];
    buffer->i_first = (buffer->i_first + 1) & CIRCULAR_BUFFER_MASK;

    return BUFFER_OK;
}

/**
 * @brief Reads multiple bytes on the buffer. The bytes are removed from the buffer.
 *
 * @param buffer [IN]: Buffer to be read from.
 * @param data [OUT]: Saves the data read from the buffer.
 * @param max_length [IN]: Max length (quantity in bytes) to read.
 * @retval uint16_t: Quantity of bytes successfully read.
 */
uint16_t Circular_Buffer_Read_Array(volatile circular_buffer_t * buffer, uint8_t * data, uint16_t max_length)
{
    uint16_t used_space = Circular_Buffer_Used_Space(buffer);
    if (used_space > max_length)
    {
        for (uint16_t i = 0; i < max_length; i++)
        {
            Circular_Buffer_Read_Byte(buffer, &data[i]);
        }
        return max_length;
    }
    else
    {
        for (uint16_t i = 0; i < used_space; i++)
        {
            Circular_Buffer_Read_Byte(buffer, &data[i]);
        }
        return used_space;
    }
    return BUFFER_OK;
}

/**
 * @brief Peeks the next byte to be read from the buffer. The byte in buffer is not removed (non-destructive).
 *
 * @param buffer [IN]: Buffer to be inspected.
 * @param byte [OUT]: Pointer where the next byte will be stored.
 * @retval buffer_status_e: BUFFER_OK if byte is available, BUFFER_EMPTY if not.
 */
buffer_status_e Circular_Buffer_Peek_Byte(volatile circular_buffer_t * buffer, uint8_t * byte)
{
    if (Circular_Buffer_Is_Empty(buffer))
        return BUFFER_EMPTY;

    *byte = buffer->data[buffer->i_first];
    return BUFFER_OK;
}

/**
 * @brief Peeks multiple upcoming bytes to be read from the buffer.  The bytes in buffer are not removed (non-destructive).
 *
 * @param buffer [IN]: Buffer to be peeked from.
 * @param data [OUT]: Saves the data peeked from the buffer.
 * @param max_length [IN]: Max length (quantity in bytes) to peek.
 * @retval uint16_t: Quantity of bytes successfully peeked.
 */
uint16_t Circular_Buffer_Peek_Array(volatile circular_buffer_t * buffer, uint8_t * data, uint16_t max_length)
{
    uint16_t used_space = Circular_Buffer_Used_Space(buffer);

    if(used_space == 0 || max_length == 0)
    {
        return 0;
    }
    else if (used_space > max_length)
    {
        uint16_t index = buffer->i_first;

        for (uint16_t i = 0; i < max_length; i++)
        {
            data[i] = buffer->data[index];
            index = (index + 1) & CIRCULAR_BUFFER_MASK;
        }
        return max_length;
    }
    else
    {
        uint16_t index = buffer->i_first;

        for (uint16_t i = 0; i < used_space; i++)
        {
            data[i] = buffer->data[index];
            index = (index + 1) & CIRCULAR_BUFFER_MASK;
        }
        return used_space;
    }
}

/**
 * @brief Peeks the latest byte saved on the circular buffer. The byte in buffer is not removed (non-destructive).
 *
 * @param buffer [IN]: Circular buffer from where data will be read.
 * @param byte [OUT]: Pointer to the byte that must received the read data.
 * @retval buffer_status_e: Operation status, returns if the buffer is empty or
 * if the data could be read.
 */
buffer_status_e Circular_Buffer_Peek_Last(volatile circular_buffer_t * buffer, uint8_t * byte)
{
    if (buffer->i_first == buffer->i_last)
        return BUFFER_EMPTY;

    *byte = buffer->data[(buffer->i_last - 1) & CIRCULAR_BUFFER_MASK];
    return BUFFER_OK;
}
