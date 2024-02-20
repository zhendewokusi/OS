#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "stdint.h"
#define PAGE_SIZE 4096
// 书上代码是让写4GB的，但是正常这里肯定要按照能分配最多的物理页来.....我该怎么写(泪)...
#define PAGING_PAGES (mem_size>>12)


extern uint64_t mem_size;
extern unsigned char mem_map [ PAGING_PAGES ];


#endif