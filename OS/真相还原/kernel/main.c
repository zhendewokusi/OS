#include "stdint.h"
#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "descriptor.h"
#include "system.h"
#include "traps.h"
#include "string.h"
#include "thread.h"

// uint64_t mem_size = 0x100000000;

// -8968605370094895462

void k_thread_a(void* arg);

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
        __MAGIC_BREAK();
        struct task_struct* tmp = thread_start("k_thread_a", 1, k_thread_a, "argA ");

        while(1);
}


/* 在线程中运行的函数 */
void k_thread_a(void* arg)
{
        /* 用void*来通用表示参数,被调用的函数知道自己需要什么类型的参数,自己转换再用 */
        char* para = arg;
        while(1) {
                put_str(para);
        }
}