#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "stdint.h"
#define MEMO_SIZE 512
#define PAGE_SIZE 4096
#define PAGING_PAGES (MEMO_SIZE>>12)
#define 


extern unsigned char mem_map [ PAGING_PAGES ];


#endif