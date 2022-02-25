#include "crc.h"

/******************************************************************************
* Name:    CRC-8               x8+x2+x+1
* Poly:    0x07
* Init:    0x00
* Refin:   False
* Refout:  False
* Xorout:  0x00
* Note:
*****************************************************************************/
uint8_t crc8(const uint8_t* data, uint32_t length)
{
  uint8_t i;
  uint32_t num = 0;
  uint8_t crc = 0;    // Initial value
  
  if (length <= 0)
    return 0;
  
  while (length-- > 0)
  {
    crc ^= data[num++]; // crc ^= *data; data++;
    for (i = 0; i < 8; i++)
    {
      if ((crc & 0x80) > 0)
        crc = (uint8_t)((crc << 1) ^ 0x07);
      else
        crc <<= 1;
    }
  }
  return crc;
}
/******************************************************************************
* Name:    CRC-16/CCITT        x16+x12+x5+1
* Poly:    0x1021
* Init:    0x0000
* Refin:   True
* Refout:  True
* Xorout:  0x0000
* Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT
*****************************************************************************/
uint16_t crc16_ccitt(const uint8_t* data, uint32_t length)
{
  uint8_t i;
  uint32_t num = 0;
  uint16_t crc = 0;   // Initial value
  
  if (length <= 0)
    return 0;
  
  while (length-- > 0)
  {
    crc ^= data[num++]; // crc ^= *data; data++;
    for (i = 0; i < 8; ++i)
    {
      if ((crc & 1) > 0)
        crc = (uint16_t)((crc >> 1) ^ 0x8408);  // 0x8408 = reverse 0x1021
      else
        crc = (uint16_t)(crc >> 1);
    }
  }
  return crc;
}
//
///******************************************************************************
//* Name:    CRC-16/IBM          x16+x15+x2+1
//* Poly:    0x8005
//* Init:    0x0000
//* Refin:   True
//* Refout:  True
//* Xorout:  0x0000
//* Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA
//*****************************************************************************/
//uint16_t crc16_ibm(const uint8_t* data, uint32_t length)
//{
//  uint8_t i;
//  uint32_t num = 0;
//  uint16_t crc = 0;   // Initial value
//  
//  if (length <= 0)
//    return 0;
//  
//  while (length-- > 0)
//  {
//    crc ^= data[num++]; // crc ^= *data; data++;
//    for (i = 0; i < 8; ++i)
//    {
//      if ((crc & 1) > 0)
//        crc = (uint16_t)((crc >> 1) ^ 0xA001);  // 0xA001 = reverse 0x8005
//      else
//        crc = (uint16_t)(crc >> 1);
//    }
//  }
//  return crc;
//}

/******************************************************************************
* Name:    CRC-32  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
* Poly:    0x4C11DB7
* Init:    0xFFFFFFF
* Refin:   True
* Refout:  True
* Xorout:  0xFFFFFFF
* Alias:   CRC_32/ADCCP
* Use:     WinRAR,ect.
*****************************************************************************/
uint32_t crc32(const uint8_t* data, uint32_t length)
{
  uint8_t i;
  uint32_t num = 0;
  uint32_t crc = 0xffffffff;  // Initial value
  
  if (length <= 0)
    return 0;
  
  while (length-- > 0)
  {
    crc ^= data[num++];         // crc ^= *data; data++;
    for (i = 0; i < 8; ++i)
    {
      if ((crc & 1) > 0)
        crc = (uint32_t)((crc >> 1) ^ 0xEDB88320);// 0xEDB88320= reverse 0x04C11DB7
      else
        crc = (crc >> 1);
    }
  }
  return (uint32_t)~crc;
}

/******************************************************************************
* Name:    CRC-32/MPEG-2  x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1
* Poly:    0x4C11DB7
* Init:    0xFFFFFFF
* Refin:   False
* Refout:  False
* Xorout:  0x0000000
* Note:
*****************************************************************************/
uint32_t crc32_mpeg_2(const uint8_t* data, uint32_t length)
{
  uint8_t i;
  uint32_t num = 0;
  uint32_t crc = 0xffffffff;  // Initial value
  
  if (length <= 0)
    return 0;
  
  while (length-- > 0)
  {
    crc ^= (uint32_t)(data[num++]) << 24;// crc ^=(uint32_t)(*data)<<24; data++;
    for (i = 0; i < 8; ++i)
    {
      if ((crc & 0x80000000) > 0)
        crc = (crc << 1) ^ 0x04C11DB7;
      else
        crc <<= 1;
    }
  }
  return crc;
}
