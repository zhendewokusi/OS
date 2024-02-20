#include "stdint.h"
#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "descriptor.h"
#include "system.h"
#include "traps.h"
#include "string.h"


uint64_t mem_size = 0x100000000;

int main(void) {
        // 什么狗屎代码，直接操作内存拿loader获取的mem_size，头疼，之前写的loader没考虑到位，哎
        put_int(mem_size);
        put_char('\n');
        asm volatile (
        "movl $0xc009efec, %%eax\n\t"
        "movl (%%eax), %0"
        : "=r" (mem_size)
        :
        : "eax"
        );
        put_int(mem_size);
        put_char('\n');

        // 初始化
        init_all();

        while(1);
}
