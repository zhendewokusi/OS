#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "stdint.h"
#define MEMO_SIZE 512
#define PAGE_SIZE 4096
#define BITMAP_MASK 1
#define PAGING_PAGES (MEMO_SIZE>>12)

// 位图
extern unsigned char kernel_bitmap [ PAGING_PAGES ];
extern unsigned char user_bitmap [ PAGING_PAGES ];

extern void bitmap_init (unsigned char * bitmap);
extern bool bitmap_scan_test (unsigned char * bitmap,uint32_t bit_index);
extern int bitmap_scan (unsigned char * bitmap,uint32_t cnt);
extern void bitmap_set (unsigned char * bitmap,uint32_t bit_index,uint8_t val);

#endif