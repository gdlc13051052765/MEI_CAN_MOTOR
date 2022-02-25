#include "includes.h"
#include "st_flash.h" 

static uint8_t EraseFlash(uint32_t FlashAddr,uint32_t Len);
static uint8_t ReadFlash(uint32_t FlashAddr,uint8_t *rBuff,uint32_t Len);
static uint8_t WriteFlash(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite); 
static uint8_t WriteNotCheck(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite);

static uint8_t FlashBuffer[STM32FLASH_PAGE_SIZE] = {0};


const _Flash_Func stFlash_Func = 
{
  .erase 					= EraseFlash,
	.write_nocheck	= WriteNotCheck,
  .write					= WriteFlash,
  .read						= ReadFlash, 
};
 
  
/*==================================================================================
* 函 数 名： EarseFlash
* 参    数： 
* 功能描述:  擦除Flash
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-04 044124
==================================================================================*/
static uint8_t EraseFlash(uint32_t FlashAddr,uint32_t length)
{ 
	uint32_t page_count = (uint32_t)(length/STM32F105RB_PAGE_SIZE);	//计算擦除的页数量
	 
  if(length <= 0)
    return 0x00;
  
		//1、解锁FLASH
  HAL_FLASH_Unlock();
	
	//1、清空错误标志
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
	//2、擦除FLASH
	//初始化FLASH_EraseInitTypeDef
	FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = FlashAddr;
	f.NbPages = 1;
	//设置PageError
	uint32_t PageError = 0;
	//调用擦除函数
	HAL_FLASHEx_Erase(&f, &PageError);
 
	//4、锁住FLASH
  HAL_FLASH_Lock();
	  
  return 0x01;
}

/*==================================================================================
* 函 数 名： ReadFlash
* 参    数： 
* 功能描述:  读Flash
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-04 044124
==================================================================================*/
static uint8_t ReadFlash(uint32_t FlashAddr,uint8_t *rBuff,uint32_t Len)
{
  if(Len <= 0)
    return 0x00;
  
		//1、清空错误标志
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
  while (Len--)
    *rBuff++ = *(uint8_t *)FlashAddr++;     

  return 0x01;
}

/*==================================================================================
* 函 数 名： WriteNotCheck
* 参    数： 
* 功能描述:  写Flash
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-04 044124
==================================================================================*/
static uint8_t WriteNotCheck(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite)
{
	uint32_t nwrite = NumToWrite/2;
	uint32_t addrmax = STM32FLASH_END;

			//1、清空错误标志
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
	while (nwrite)
	{
		if (Address > addrmax)
			return 0x81; //失败

		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Address, *((uint16_t*)Buffer));
		if ( (*(__IO uint16_t *)Address) != *((uint16_t*)Buffer) )
			return 0x82; //失败

		nwrite--;
		Buffer += 2;
		Address += 2;
	}
	return 0x01; //成功
}

/*==================================================================================
* 函 数 名： WriteNotCheck
* 参    数： 
* 功能描述:  写Flash
* 返 回 值： None
* 备    注： 
* 作    者： xiaozh
* 创建时间： 2021-01-04 044124
==================================================================================*/
static uint8_t WriteFlash(uint32_t Address, const uint8_t *Buffer, uint32_t NumToWrite)
{
	uint32_t i = 0;
	uint8_t ret = 0;
	uint32_t pagepos = 0;         // 页位置
	uint32_t pageoff = 0;         // 页内偏移地址
	uint32_t pagefre = 0;         // 页内空余空间
	uint32_t offset = 0;          // Address在FLASH中的偏移
	uint32_t nwrite = NumToWrite; // 记录剩余要写入的数据量

			//1、清空错误标志
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | \
											 FLASH_FLAG_PGERR | \
											 FLASH_FLAG_WRPERR);
	
	/* 非法地址 */
	if (Address < STM32FLASH_BASE || Address > (STM32FLASH_END) || NumToWrite == 0 || Buffer == NULL)
		return 0;

	/* 解锁FLASH */
	HAL_FLASH_Unlock();

	/* 计算偏移地址 */
	offset = Address - STM32FLASH_BASE;

	/* 计算当前页位置 */
	pagepos = offset / STM32FLASH_PAGE_SIZE;

	/* 计算要写数据的起始地址在当前页内的偏移地址 */
	pageoff = (offset % STM32FLASH_PAGE_SIZE);

	/* 计算当前页内空余空间 */
	pagefre = ((STM32FLASH_PAGE_SIZE) - pageoff);

	/* 要写入的数据量低于当前页空余量 */
	if (nwrite <= pagefre)
		pagefre = nwrite;

	while (nwrite != 0)
	{
		/* 检查是否超页 */
		if (pagepos >= STM32FLASH_PAGE_NUM)
			break;

		/* 读取一页 */
		ReadFlash(STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE, FlashBuffer, STM32FLASH_PAGE_SIZE >> 1);

		/* 检查是否需要擦除 */
		for (i = 0; i < pagefre; i++)
		{
			if (*(FlashBuffer + pageoff + i) != 0xFF) /* FLASH擦出后默认内容全为0xFF */
				break;
		}

		if (i < pagefre)
		{
			uint8_t count = 0;
			uint32_t index = 0;
			uint32_t PageError = 0;
			FLASH_EraseInitTypeDef pEraseInit;

			/* 擦除一页 */
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			pEraseInit.PageAddress = STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE;
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.NbPages = 1;
			if (HAL_FLASHEx_Erase(&pEraseInit, &PageError) != HAL_OK)
				break;

			/* 复制到缓存 */
			for (index = 0; index < pagefre; index++)
			{
				*(FlashBuffer + pageoff + index) = *(Buffer + index);
			}

			/* 写回FLASH */
			ret = WriteNotCheck(STM32FLASH_BASE + pagepos * STM32FLASH_PAGE_SIZE, FlashBuffer, STM32FLASH_PAGE_SIZE);
			if (ret == (0x01))
			{
				nwrite--;
				break;
			}
		}
		else
		{
			/* 无需擦除，直接写 */
			ret = WriteNotCheck(Address, Buffer, pagefre);
			if (ret != 0x01)
			{
				nwrite--;
				break;
			}
		}

		Buffer += pagefre;         /* 读取地址递增         */
		Address += (pagefre << 1); /* 写入地址递增         */
		nwrite -= pagefre;         /* 更新剩余未写入数据量 */

		pagepos++;   /* 下一页               */
		pageoff = 0; /* 页内偏移地址置零     */

		/* 根据剩余量计算下次写入数据量 */
		pagefre = nwrite >= (STM32FLASH_PAGE_SIZE >> 1) ? (STM32FLASH_PAGE_SIZE >> 1) : nwrite;
	}

	/* 加锁FLASH */
	HAL_FLASH_Lock();

	return ((NumToWrite - nwrite) << 1);
}
