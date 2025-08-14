/**
 * @file message_buffer.h
 *
 * @brief Module to store/read messages.
 *
 * Its similar to circular_buffer module, but this is more useful to use
 * with USART transmission messages, by creating a FIFO message buffer.
 */

#ifndef UTILS_MESSAGE_BUFFER_H_
#define UTILS_MESSAGE_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "circular_buffer.h"

#define MESSAGE_INDEX_STACK_SIZE 64  // Ajuste conforme necessário

/**
 * @brief Struct definition of the Circular Buffer.
 *
 */
typedef struct
{
    circular_buffer_t data;                         /**< Data Buffer */
    uint16_t quant_msg;                             /**< Quantity of messages */
    uint16_t msg_indexes[MESSAGE_INDEX_STACK_SIZE]; /**< Stack of message start indices */
    uint16_t stack_head;                            /**< Points to where the next index will be stored */
    uint16_t stack_size;                            /**< How many valid indices are in the stack */
} message_buffer_t;


/**
 * @brief Init variables of the message buffer.
 *
 * @param msg_buffer [IN]: Message buffer to be initialized.
 */
void Message_Buffer_Init(volatile message_buffer_t * msg_buffer);

/**
 * @brief Verify if the message buffer is empty.
 *
 * @param msg_buffer [IN]: Message buffer to be analyzed.
 *
 * @retval 1 if empty, 0 if not empty.
 */
uint8_t Message_Buffer_Is_Empty(volatile message_buffer_t * msg_buffer);

/**
 * @brief Get amount of bytes saved on the buffer.
 *
 * Obs: for each message, there is two additional bytes that stores the message length.
 *
 * @param msg_buffer [IN]: Message buffer to be analyzed.
 *
 * @retval uint16_t quantity of bytes saved on the buffer.
 */
uint16_t Message_Buffer_Used_Space(volatile message_buffer_t * msg_buffer);

/**
 * @brief Get the available space (bytes) in the buffer (i.e., how many bytes can still be written).
 *
 * Obs: for each message, there is two additional bytes that stores the message length.
 *
 * @param msg_buffer [IN]: Message buffer to be analyzed.
 *
 * @retval uint16_t number of bytes available to write.
 */
uint16_t Message_Buffer_Available_Space(volatile message_buffer_t * msg_buffer);

/**
 * @brief Get the quantity of messages in the buffer.
 *
 * @param msg_buffer [IN]: Message buffer to be analyzed.
 *
 * @retval uint16_t quantity of messages in the buffer.
 */
uint16_t Message_Buffer_Quant_Msg(volatile message_buffer_t *msg_buffer);

/**
 * @brief Clear the buffer.
 *
 * @param msg_buffer [IN]: buffer to be cleared.
 */
void Message_Buffer_Flush(volatile message_buffer_t * msg_buffer);

/**
 * @brief Writes a new message to the buffer.
 *
 * @param msg_buffer [IN]: Message buffer to receive the message
 * @param message [IN]: message (data array) to be stored on the buffer
 * @param length [IN]: length (quantity in bytes) of the message.
 *
 * @retval buffer_status_e: Operation status, returns if the buffer is
 * full or if the data could be saved.
 */
buffer_status_e Message_Buffer_Write_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t length);

/**
 * @brief Reads a message from the buffer. The message is removed from the buffer.
 *
 * @param msg_buffer [IN]: Message buffer to be read from.
 * @param *message [OUT]: message (data array) read.
 * @param length [OUT]: length of the message.
 *
 * @retval buffer_status_e: Operation status, returns if the buffer is
 * empty or if the data could be read.
 */
buffer_status_e Message_Buffer_Read_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length);

/**
 * @brief Peeks a message from the buffer. The message is not removed from the buffer (non-destructive).
 *
 * @param msg_buffer [IN]: Message buffer to be peeked from.
 * @param *message [OUT]: message (data array) peeked.
 * @param length [OUT]: length of the message.
 *
 * @retval buffer_status_e: Operation status, returns if the buffer is
 * empty or if the data could be peeked.
 */
buffer_status_e Message_Buffer_Peek_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length);

/**
 * @brief Peeks a message from the buffer. The message is not removed from the buffer (non-destructive).
 *
 * @param msg_buffer [IN]: Message buffer to be peeked from.
 * @param *message [OUT]: message (data array) peeked.
 * @param length [OUT]: length of the message.
 *
 * @retval buffer_status_e: Operation status, returns if the buffer is
 * empty or if the data could be peeked.
 */
buffer_status_e Message_Buffer_Peek_Last_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length);


#ifdef __cplusplus
}
#endif

#endif /* UTILS_MESSAGE_BUFFER_H_ */
