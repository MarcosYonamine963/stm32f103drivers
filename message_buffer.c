#include "message_buffer.h"

void Message_Buffer_Init(volatile message_buffer_t * msg_buffer)
{
    Circular_Buffer_Init(&msg_buffer->data);
    msg_buffer->quant_msg = 0;
}

uint8_t Message_Buffer_Is_Empty(volatile message_buffer_t * msg_buffer)
{
    return (msg_buffer->quant_msg == 0);
}

uint16_t Message_Buffer_Used_Space(volatile message_buffer_t * msg_buffer)
{
    return Circular_Buffer_Used_Space(&msg_buffer->data);
}

uint16_t Message_Buffer_Available_Space(volatile message_buffer_t * msg_buffer)
{
    return Circular_Buffer_Available_Space(&msg_buffer->data);
}

uint16_t Message_Buffer_Quant_Msg(volatile message_buffer_t *msg_buffer)
{
    return msg_buffer->quant_msg;
}

void Message_Buffer_Flush(volatile message_buffer_t * msg_buffer)
{
    Circular_Buffer_Flush(&msg_buffer->data);
    msg_buffer->quant_msg = 0;
}

buffer_status_e Message_Buffer_Write_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t length)
{
    uint16_t required_space = length + 2; // 2 bytes para o tamanho
    if (Circular_Buffer_Available_Space(&msg_buffer->data) < required_space)
        return BUFFER_FULL;

    uint8_t size_header[2] = { (uint8_t)(length >> 8), (uint8_t)(length & 0xFF) };

    if (Circular_Buffer_Write_Array(&msg_buffer->data, size_header, 2) != BUFFER_OK)
        return BUFFER_FULL;

    if (Circular_Buffer_Write_Array(&msg_buffer->data, message, length) != BUFFER_OK)
    {
        // rollback do header
        msg_buffer->data.i_last = (msg_buffer->data.i_last - 2) & CIRCULAR_BUFFER_MASK;
        return BUFFER_FULL;
    }

    msg_buffer->quant_msg++;
    return BUFFER_OK;
}

buffer_status_e Message_Buffer_Read_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length)
{
    uint8_t size_header[2];

    if (msg_buffer->quant_msg == 0)
        return BUFFER_EMPTY;

    if (Circular_Buffer_Peek_Array(&msg_buffer->data, size_header, 2) < 2)
        return BUFFER_EMPTY;

    *length = ((uint16_t)size_header[0] << 8) | size_header[1];

    if (Circular_Buffer_Used_Space(&msg_buffer->data) < (*length + 2))
        return BUFFER_EMPTY;

    // Flush the message header:
    Circular_Buffer_Read_Array(&msg_buffer->data, size_header, 2);

    // Save the message data
    Circular_Buffer_Read_Array(&msg_buffer->data, message, *length);
    msg_buffer->quant_msg--;

    return BUFFER_OK;
}

// // essa versao da funcao retorna a mensagem junto com os 2 bytes de header do tamanho da mensagem.
//buffer_status_e Message_Buffer_Peek_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length)
//{
//    if (msg_buffer->quant_msg == 0)
//        return BUFFER_EMPTY;
//
//    uint8_t size_header[2];
//    if (Circular_Buffer_Peek_Array(&msg_buffer->data, size_header, 2) != BUFFER_OK)
//        return BUFFER_EMPTY;
//
//    *length = ((uint16_t)size_header[0] << 8) | size_header[1];
//
//    if (Circular_Buffer_Used_Space(&msg_buffer->data) < (*length + 2))
//        return BUFFER_EMPTY;
//
//    return Circular_Buffer_Peek_Array(&msg_buffer->data, message, *length + 2) == BUFFER_OK
//               ? Circular_Buffer_Peek_Array(&msg_buffer->data, message, *length + 2) == BUFFER_OK
//                     ? Circular_Buffer_Peek_Array(&msg_buffer->data, message + 2, *length)
//                     : BUFFER_EMPTY
//               : BUFFER_EMPTY;
//}

buffer_status_e Message_Buffer_Peek_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length)
{
    if (msg_buffer->quant_msg == 0)
        return BUFFER_EMPTY;

    uint8_t size_header[2];

    // Lê os dois bytes iniciais (tamanho da mensagem)
    if (Circular_Buffer_Peek_Array(&msg_buffer->data, size_header, 2) < 2)
        return BUFFER_EMPTY;

    *length = ((uint16_t)size_header[0] << 8) | size_header[1];

    // Verifica se há espaço suficiente para o payload
    if (Circular_Buffer_Used_Space(&msg_buffer->data) < (*length + 2))
        return BUFFER_EMPTY;

    // Cria buffer temporário para header + payload
    uint8_t temp[2 + *length];

    // Copia tudo (header + payload) para temp
    if (Circular_Buffer_Peek_Array(&msg_buffer->data, temp, 2 + *length) < 2 + *length)
        return BUFFER_EMPTY;

    // Copia apenas o payload para o buffer fornecido
    for (uint16_t i = 0; i < *length; i++)
        message[i] = temp[2 + i];

    return BUFFER_OK;
}

// TODO verificar essa funcao
buffer_status_e Message_Buffer_Peek_Last_Message(volatile message_buffer_t *msg_buffer, uint8_t *message, uint16_t *length)
{
    if (msg_buffer->quant_msg == 0)
        return BUFFER_EMPTY;

    circular_buffer_t *buf = (circular_buffer_t *)&msg_buffer->data;
    uint16_t idx = buf->i_first;
    uint16_t used = Circular_Buffer_Used_Space(buf);
    uint16_t count = 0;
    uint16_t last_msg_pos = 0;
    uint16_t msg_len = 0;

    while (count + 2 <= used && last_msg_pos < used)
    {
        // Lê o header de 2 bytes
        uint8_t size_high = buf->data[(idx + 0) & CIRCULAR_BUFFER_MASK];
        uint8_t size_low  = buf->data[(idx + 1) & CIRCULAR_BUFFER_MASK];
        uint16_t size = ((uint16_t)size_high << 8) | size_low;

        if (count + 2 + size > used)
            break;  // Mensagem incompleta ou corrompida

        last_msg_pos = idx;     // Guarda início da última mensagem válida
        msg_len = size;

        // Avança para a próxima mensagem
        idx = (idx + 2 + size) & CIRCULAR_BUFFER_MASK;
        count += 2 + size;
    }

    if (msg_len == 0)
        return BUFFER_EMPTY;

    *length = msg_len;

    // Copia apenas os dados da última mensagem (sem header)
    for (uint16_t i = 0; i < msg_len; i++)
    {
        message[i] = buf->data[(last_msg_pos + 2 + i) & CIRCULAR_BUFFER_MASK];
    }

    return BUFFER_OK;
}
