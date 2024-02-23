#include "stdint.h"
#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "descriptor.h"
#include "system.h"
#include "traps.h"
#include "string.h"


// uint64_t mem_size = 0x100000000;


int main(void) {
        // 初始化
        init_all();
        void* addr = get_kernel_pge(3);
        put_str("\n     get_kernel_page start vaddr is ");
        put_int((uint32_t)addr);
        void* addr2 = get_kernel_pge(2);
        put_str("\n     get_kernel_page start vaddr2 is ");
        put_int((uint32_t)addr2);
        put_str("\n");
        while(1);
}
