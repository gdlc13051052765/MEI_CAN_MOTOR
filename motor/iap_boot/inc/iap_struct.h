#ifndef  __IAP_STRUCT_H
#define  __IAP_STRUCT_H

#include <stdint.h>

//flash接口定义
typedef struct
{
	uint8_t (*init)(void);
  uint8_t (*erase)(uint32_t, uint32_t);
  uint8_t (*write_nocheck)(uint32_t, const uint8_t*, uint32_t);		
	uint8_t (*write)(uint32_t, const uint8_t*, uint32_t);
  uint8_t (*read)(uint32_t, uint8_t*, uint32_t); 
}_Flash_Func,*_pFlash_Func;



#endif
