#ifndef  __CAN_DRV_H
#define  __CAN_DRV_H
#include <stdint.h>
#include "main.h"


typedef volatile unsigned char  VU08;   /*unsigned 8 bit definition */
typedef volatile unsigned int   VU16;   /*unsigned 16 bit definition*/
typedef volatile unsigned long  VU32;   /*unsigned 32 bit definition*/




void can_drv_init(void *fifo, uint8_t type);
uint8_t  can_sed_pkg_without_cache(CAN_HandleTypeDef can, void* can_msg);
#endif
