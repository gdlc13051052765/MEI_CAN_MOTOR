#ifndef  __ST_CRC_H
#define  __ST_CRC_H
#include <stdint.h>


void st_crc_init(void);
uint32_t st_crc32(uint8_t *buff, uint16_t len);
void st_crc32_reset(void);
uint32_t st_crc32_no_reset(uint8_t *buff, uint16_t len);
#endif
