#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "descriptor.h"
#include "system.h"
#include "traps.h"
#include "string.h"

int mem_size = 0;

int main(void) {
        // 什么狗屎代码，直接操作内存拿loader获取的mem_size，头疼，之前写的loader没考虑到位，哎
        asm volatile (
        "movl $0xc009efec, %%eax\n\t"
        "movl (%%eax), %0"
        : "=r" (mem_size)
        :
        : "eax"
        );
        // 初始化
        init_all();

        while(1);
}
