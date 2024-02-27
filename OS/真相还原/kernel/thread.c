#include "thread.h"
#include "traps.h"
#include "timer.h"
#include "memory.h"
#include "string.h"

#define PAGE_SIZE 4096
#define TASK_PAGE_SIZE 1

typedef void thread_func(void*) ;


static void kernel_thread(thread_func* func, void* func_arg)
{
        func(func_arg);
}

void thread_create(struct task_struct * pthread, thread_func function, void* func_arg)
{
        pthread->self_kstack -= sizeof(struct intr_stack);
        pthread->self_kstack -= sizeof(struct thread_stack);
        struct thread_stack * kthread_stack = (struct thread_stack *)pthread->self_kstack;
        kthread_stack->eip = kernel_thread;
        kthread_stack->func = function;
        kthread_stack->func_arg = func_arg;
        kthread_stack->ebp = kthread_stack->ebx = kthread_stack->edi = kthread_stack->esi = 0;
}

void init_thread(struct task_struct * pthread,char * name,uint8_t priority)
{
        memset(pthread,0,sizeof(*pthread));
        strcpy(pthread->name,name);
        pthread->status = TASK_READY;
        pthread->priority = priority;
        pthread->stack_magic = 0x12345678;      // 魔数
}

struct task_struct* thread_start(char* name,uint8_t priority,thread_func function,void* func_arg)
{
        struct task_struct* thread = get_kernel_pge(TASK_PAGE_SIZE);
        init_thread(thread, name, priority);
        thread_create(thread, function, func_arg);
        __asm__ __volatile__ ("movl %0, %%esp; \
        pop %%ebp;pop %%ebx;pop %%edi;pop %%esi; \
        ret": :"g" (thread->self_kstack) : "memory");
        return thread;
}