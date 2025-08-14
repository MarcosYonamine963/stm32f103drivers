#ifndef UTILS_CONVERT_H_
#define UTILS_CONVERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Convert_Byte_HAscii(uint8_t byte, uint8_t* result);
void Convert_Uint16_Ascii(uint16_t num, uint8_t* result);
void Convert_Uint32_Ascii(uint32_t num, uint8_t* result);
uint8_t Convert_HAscii_Byte(uint8_t* buffer);
uint8_t Convert_Ascii_Uint8(uint8_t * ascii, uint8_t len);
uint16_t Convert_Ascii_Uint16(uint8_t * ascii, uint8_t len);
uint32_t Convert_Ascii_Uint32(uint8_t * ascii, uint8_t len);
void Convert_Uint8_Ascii(uint8_t num, uint8_t *result);


#ifdef __cplusplus
}
#endif

#endif /* UTILS_CONVERT_H_ */
