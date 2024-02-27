#ifndef __THREAD_H__
#define __THREAD_H__

#include "stdint.h"

typedef void thread_func(void*) ;

enum task_status {
        TASK_RUNNING,   // 任务运行中
        TASK_READY,     // 任务就绪
        TASK_DIED,      // 任务死亡
        TASK_WAITING,

};

// 中断发生时候保护上下文环境
// 该结构在线程自己的内核栈中的位置是固定的，所以在页的顶端
// 栈，从下往上看
struct intr_stack {
        uint32_t vector; // 中断号
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp_dummy;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
        uint32_t gs;
        uint32_t fs;
        uint32_t es;
        uint32_t ds;
        uint32_t error_code;

        void(*eip)(void);
        uint32_t cs;
        uint32_t eflags;
        void* esp;
        uint32_t ss;
};

struct thread_stack {
//      ABI 标准
        uint32_t ebp;
        uint32_t ebx;
        uint32_t edi;
        uint32_t esi;

        void (*eip)(thread_func* func,void * func_arg);

        // 仅用于第一次被调用时使用
        void(*unused_retaddr);  // 占位
        // 正常一个函数是通过 call 进去的，栈顶当时必须是ret_addr，我们写的线程不需要这个返回地址，只需要一路向前就行了，但是进去的函数也是符合汇编调用函数的标准的，就必须有一个占位的地方，才能正常基于此的偏移地址来访问正确数据
        thread_func* func;
        void* func_arg;
};

// 进程或者线程的PCB
struct task_struct {
        uint32_t * self_kstack;
        enum task_status status;
        uint8_t priority;
        char name[16];
        uint32_t stack_magic;
};
void thread_create(struct task_struct * pthread, thread_func function, void* func_arg);
void init_thread(struct task_struct * pthread,char * name,uint8_t priority);
struct task_struct* thread_start(char* name,uint8_t priority,thread_func function,void* func_arg);
#endif