/**
    @defgroup memory
    @ingroup kernel_api

    �ں˱�̽ӿ� �ڴ��������

    @{
*/

#ifndef KE_MEMORY_H
#define KE_MEMORY_H

#include <compiler.h>
typedef unsigned int page_prot_t;
#define KM_PROT_READ	(1 << 0)
#define KM_PROT_WRITE	(1 << 1)
#define KM_MAP_DEVICE	(1 << 16)

/**
	@brief Map IO memory space
 */
DLLEXPORT void *km_map_physical(unsigned long physical, size_t size, unsigned long flags);
#define KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL	(1 << 0)				/* �����ַ�󶨵��ض������ַ */
#define KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE	(1 << 1)				/* �����ַӳ��ʱ��ʹ�ó����cacheģʽ�������Ƿǻ��棩*/

/**
	@brief Allocate a kernel page
*/
DLLEXPORT void *km_alloc_kernel_page();
#endif

/** @} */
