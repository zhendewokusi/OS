#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "stdint.h"
#define MEMO_SIZE 512
#define BITMAP_MASK 1
#define PAGING_PAGES ((MEMO_SIZE * 1024 * 1024 )>> 12 )

struct bitmap {
   uint32_t btmp_bytes_len;
   uint8_t* bits;
};


extern void bitmap_init (struct bitmap* btmp);
extern bool bitmap_scan_test (struct bitmap* btmp,uint32_t bit_index);
extern int bitmap_scan (struct bitmap* btmp,uint32_t cnt);
extern void bitmap_set (struct bitmap* btmp,uint32_t bit_index,uint8_t val);

#endif