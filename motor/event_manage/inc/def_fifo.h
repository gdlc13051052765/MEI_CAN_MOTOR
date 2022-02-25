#ifndef  __DEF_FIFO_H
#define  __DEF_FIFO_H
#include <stdint.h>
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#define SAFE_ATOM_CODE(code) 		code
#define DEF_CLASS								typedef struct{
#define END_DEF_CLASS(name)	 		}name;
#define CLASS(type)							type

#define FIFO_MUTEX(__NAME, __pFIFO)\
				__NAME##_fifo_mutex(__pFIFO)
				
#define FIFO_INIT_S(__NAME, __pFIFO, __BUFFER, __SIZE)\
				__NAME##_fifo_init_static((__pFIFO), (__BUFFER), (__SIZE))

#define FIFO_INIT_M(__NAME, __pFIFO, __BUFFER, __SIZE)\
				__NAME##_fifo_init_malloc((__pFIFO), (__BUFFER), (__SIZE))
				
#define IS_FIFO_EMPTY(__NAME, __pFIFO)\
				is_##__NAME##_fifo_empty((__pFIFO))
				
#define FIFO_PUSH(__NAME, __pFIFO, __pITEM)\
				__NAME##_push_item(__pFIFO, __pITEM)

#define FIFO_POP(__NAME, __pFIFO, __pITEM)\
				__NAME##_pop_item(__pFIFO, __pITEM)
				
#define FIFO(__NAME)						__NAME##_fifo_t
#define FIFO_NAME(__NAME)				__NAME##_FIFO_t


#define FIFO_INIT_S_X(__NAME)  			__NAME##_fifo_init_static
#define FIFO_INIT_M_X(__NAME)  			__NAME##_fifo_init_malloc
#define FIFO_IS_EMPTY_X(__NAME)  		is_##__NAME##_fifo_empty
#define FIFO_PUSH_X(__NAME)  				__NAME##_push_item
#define FIFO_POP_X(__NAME)  				__NAME##_pop_item


//2019-11-06 155944
#define DEF_FIFO_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, __ATOM_ACCESS)							\
DEF_CLASS			 																																						\
		__TYPE				*data;																																	\
		__PTR_TYPE		max_size;																																\
		__PTR_TYPE		head;																																		\
		__PTR_TYPE		tail;																																		\
		__PTR_TYPE		item_num;																																\
		__MUTEX_TYPE	mutex;																																	\
END_DEF_CLASS(__NAME##_fifo_t)																														\
																																													\
__MUTEX_TYPE *__NAME##_fifo_mutex(void *pfifo)																						\
{																																													\
	CLASS(__NAME##_fifo_t) *pthis = (CLASS(__NAME##_fifo_t) *)pfifo;												\
	if(0 == pthis)																																					\
	{return 0;}																																							\
	return &(pthis->mutex);																																	\
}																																													\
																																													\
uint8_t __NAME##_fifo_init_static(void *pfifo, __TYPE *pbuffer, __PTR_TYPE size)					\
{																																													\
	CLASS(__NAME##_fifo_t) *pthis = (CLASS(__NAME##_fifo_t) *)pfifo;												\
	if((0==pthis) || (0==pbuffer) || (0==size))																							\
	{return 0;}																																							\
																																													\
	pthis->data 		= pbuffer;																															\
	pthis->max_size = size;																																	\
	pthis->head			= 0;																																		\
	pthis->tail			= 0;																																		\
	pthis->item_num	= 0;																																		\
																																													\
	return 0x01;																																						\
}																																													\
																																													\
void* __NAME##_fifo_init_malloc( __TYPE *pbuffer, __PTR_TYPE size)												\
{																																													\
	CLASS(__NAME##_fifo_t) *pthis = (CLASS(__NAME##_fifo_t) *)calloc(1,sizeof(CLASS(__NAME##_fifo_t)));\
	if((0==pthis) || (0==pbuffer) || (0==size))																							\
	{return 0;}																																							\
																																													\
	pthis->data 		= pbuffer;																															\
	pthis->max_size = size;																																	\
	pthis->head			= 0;																																		\
	pthis->tail			= 0;																																		\
	pthis->item_num	= 0;																																		\
																																													\
	return pthis;																																						\
}																																													\
																																													\
uint8_t is_##__NAME##_fifo_empty(void *pfifo)																							\
{																																													\
	CLASS(__NAME##_fifo_t) *pthis = (CLASS(__NAME##_fifo_t) *)pfifo;												\
	if(0==pthis)																																						\
	{return 0;}																																							\
																																													\
	if(pthis->head == pthis->tail)																													\
	{return 0x00;}																																					\
	else																																										\
	{return 0x01;}																																					\
}																																													\
																																													\
uint8_t __NAME##_push_item(void *pfifo, __TYPE *pItem)																		\
{																														\
	CLASS(__NAME##_fifo_t) *pthis = (CLASS(__NAME##_fifo_t) *)pfifo;												\
	if((0==pthis) || (0==pItem))																														\
	{return 0;}																																							\
																																													\
	__ATOM_ACCESS(																																					\
		do{																																										\
				if(pthis->tail == ((pthis->head+1)%pthis->max_size)) 														  \
				{	return 0x00;}																																		\
																																													\
				memcpy((void*)&pthis->data[pthis->head], (const void*)pItem, sizeof(__TYPE));			\
				pthis->head++;																																		\
				pthis->head = pthis->head % pthis->max_size;																			\
				pthis->item_num++;																																\
				return 0x01;																																			\
			}while(0);		)																																			\
																																													\
}                                                                                         \
																																													\
uint8_t __NAME##_pop_item(void *pfifo, __TYPE *pItem)																			\
{																																													\
	CLASS(__NAME##_fifo_t) *pthis = (CLASS(__NAME##_fifo_t) *)pfifo;												\
	if((0==pthis) || (0==pItem))																														\
	{return 0;}																																							\
																																													\
		__ATOM_ACCESS(																																				\
		do{																																										\
				if(pthis->head == pthis->tail) 														  											\
				{	return 0x00;}																																		\
																																													\
				memcpy((void*)pItem, (const void*)&pthis->data[pthis->tail], sizeof(__TYPE));			\
				pthis->tail++;																																		\
				pthis->tail = pthis->tail % pthis->max_size;																			\
				(pthis->item_num>0)?(pthis->item_num--):(pthis->item_num=0);											\
				return 0x01;																																			\
			}while(0);		)																																			\
																																													\
}																																													\

#define DEF_SAFE_FIFO(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)            \
        DEF_FIFO_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, SAFE_ATOM_CODE)

#define DEF_SAFE_FIFO_U8(__NAME, __PTR_TYPE,__MUTEX_TYPE)                  \
        DEF_SAFE_FIFO(__NAME, uint8_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_SAFE_FIFO_U16(__NAME, __PTR_TYPE, __MUTEX_TYPE)                \
        DEF_SAFE_FIFO(__NAME, uint16_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_SAFE_FIFO_END


#define DEF_QUEUE(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE)                 \
        DEF_FIFO_EX(__NAME, __TYPE, __PTR_TYPE, __MUTEX_TYPE, NONE_ATOM_ACCESS)

#define DEF_QUEUE_U8(__NAME, __PTR_TYPE,__MUTEX_TYPE)                       \
        DEF_QUEUE(__NAME, uint8_t, __PTR_TYPE, __MUTEX_TYPE)

#define DEF_QUEUE_END

#endif
