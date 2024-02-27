#include "threads.h"
#include "traps.h"
#include "timer.h"
#include "memory.h"
#include "string.h"

#define PAGE_SIZE 4096

static void kernel_thread(thread_func* func, void* func_arg)
{
        func(func_arg);
}

void thread_create(struct task_struct * pthread, thread_func function, void* func_arg)
{
        pthread->self_kstack -= sizeof(struct intr_stack);
        pthread->self_kstack -= sizeof(struct thread_stack);
        struct thread_stack * kthread_stack = (struct thread_stack *)pthread->self_kstack;
        pthread->eip = kernel_thread;
        pthread->func = function;
        pthread->func_arg = func_arg;
        pthread->ebp = pthread->ebx = pthread->edi = pthread->esi = 0;
}

void init_thread(struct tast_struct * pthread,char * name,uint8_t priority)
{
        memset(pthread,0,sizeof(*pthread));
        strcpy(pthread->name,name);
        pthread->status = TASK_READY;
        pthread->priority = priority;
        pthread->stack_magic = 0x12345678;      // 魔数
}