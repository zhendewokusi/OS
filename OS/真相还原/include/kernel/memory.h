#ifndef __MEMORY_H__
#define  __MEMORY_H__
#include "stdint.h"
#include "bitmap.h"

enum pool_flags {
   PF_KERNEL = 1,    // 内核内存池
   PF_USER = 2	     // 用户内存池
};

#define	 PG_P_1	  1	// 页表项或页目录项存在属性位
#define	 PG_P_0	  0	// 页表项或页目录项存在属性位
#define	 PG_RW_R  0	// R/W 属性位值, 读/执行
#define	 PG_RW_W  2	// R/W 属性位值, 读/写/执行
#define	 PG_US_S  0	// U/S 属性位值, 系统级
#define	 PG_US_U  4	// U/S 属性位值, 用户级

// 用于虚拟地址管理
struct virtual_addr {
   struct bitmap vaddr_bitmap;
   uint32_t vaddr_start;
};

void memo_init();
static void mem_pool_init(uint32_t all_mem);

#endif
