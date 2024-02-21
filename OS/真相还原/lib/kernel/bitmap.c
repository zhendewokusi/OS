#include "bitmap.h"
#include "stdint.h"
#include "traps.h"
#include "string.h"

extern void bitmap_init (unsigned char * bitmap)
{
        memset(bitmap, 0, PAGING_PAGES);
}

extern bool bitmap_scan_test (unsigned char * bitmap,uint32_t bit_index)
{
        uint32_t byte_i = bit_index / 8;
        uint32_t bit_i  = bit_index % 8;
        return (bitmap[byte_i] & (BITMAP_MASK << bit_i));
}

// 在位图中申请连续cnt个位,成功则返回其起始位下标，失败返回-1
extern int bitmap_scan (unsigned char * bitmap,uint32_t cnt)
{
        uint32_t idx_byte = 0;
        while ((0xff == bitmap[idx_byte]) && (idx_byte < PAGING_PAGES) ) {
                idx_byte++;
        }
        ASSERT(idx_byte < PAGING_PAGES);
        if (idx_byte == PAGING_PAGES) {
                return -1;
        }

        uint32_t idx_bit = 0;
        while ((uint8_t)(BITMAP_MASK << idx_bit) & bitmap[idx_byte]) {
                idx_bit++;
        }
        int bit_idx_start = idx_byte * 8 + idx_bit;
        if(cnt == 1) {
                return bit_idx_start;
        }

        uint32_t bit_left = (PAGING_PAGES * 8 - bit_idx_start);
        uint32_t next_bit = bit_idx_start + 1;
        uint32_t count = 1;

        bit_idx_start = -1;
        while(bit_left-- > 0) {
                if(!bitmap_scan_test(bitmap, next_bit)) {
                        count++;
                } else {
                        count = 0;
                }
                if(count == cnt) {
                        bit_idx_start = next_bit - cnt + 1;
                        break;
                }
                next_bit++;
        }
        return bit_idx_start;
}

extern void bitmap_set (unsigned char * bitmap,uint32_t bit_index,uint8_t val) {
        ASSERT((val == 0) || (val == 1));
        uint32_t byte_idx = bit_index / 8;
        uint32_t bit_idx  = bit_index % 8;
        if (val) {
                bitmap[byte_idx] |= (BITMAP_MASK << bit_idx);
        } else {
                bitmap[byte_idx] &= ~(BITMAP_MASK << bit_idx);
        }
}
