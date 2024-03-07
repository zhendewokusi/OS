#include "thread.h"
#include "traps.h"
#include "timer.h"
#include "memory.h"
#include "string.h"
#include "kernel.h"

#define PAGE_SIZE 4096
#define TASK_PAGE_SIZE 1

typedef void thread_func(void*) ;

// 获取当前运行线程的PCB
void* get_runthread_kstack()
{
	uint32_t esp = 0;
	__asm__ __volatile__ ("mov %%esp,%0" : "=g" (esp));
	return (esp & 0xfffff000);
}

static void kernel_thread(thread_func* func, void* func_arg)
{
	func(func_arg);
}

// 线程 thread_stack 信息
void thread_create(struct thread_info * thread_info, thread_func function, void* func_arg)
{
	thread_info->self_kstack -= sizeof(struct intr_stack);
	thread_info->self_kstack -= sizeof(struct thread_stack);
	struct thread_stack * kthread_stack = (struct thread_stack *)thread_info->self_kstack;
	kthread_stack->eip = kernel_thread;
	kthread_stack->func = function;
	kthread_stack->func_arg = func_arg;
	kthread_stack->ebp = kthread_stack->ebx = kthread_stack->edi = kthread_stack->esi = 0;

}

uint8_t have_init = 0;

void init_thread(struct thread_info * thread_info,char * name,uint8_t priority)
{
	memset(thread_info,0,sizeof(struct thread_info));
	thread_info->self_kstack = (uint32_t *)((uint32_t)thread_info + PAGE_SIZE);

	if(unlikely((strcmp(name, "init") == 0) && !have_init)) {
		thread_info->status = TASK_RUNNING;
		// 初始化 kernel 的 PCB
		struct task_struct * task = (struct task_struct *)((uint32_t)(get_runthread_kstack()) - 0x1000);
		thread_info->task = task;
		INIT_LIST_HEAD(&task->thread_group);
	} else {
		thread_info->status = TASK_READY;
		// 给新线程创建 task_struct
		struct task_struct* task = get_kernel_pge(TASK_PAGE_SIZE);
		thread_info->task = task;
	}
	strcpy(thread_info->task->name,name);
	thread_info->task->priority = priority;
	thread_info->stack_magic = 0x21549844;		// 魔数

	struct task_struct* leader = ((struct thread_info *)get_runthread_kstack())->task;
	thread_info->task->group_leader = leader;
	// 将线程加入到当前进程所在的 task_struct 的 thread_group 中
	// 这里不对，如果内核创建线程，这里加不进去。
	if(likely(have_init)) {
		list_add_tail(&thread_info->task->thread_group,&leader->thread_group);
	} else {
		have_init++;
	}
}

// 创建并且进入该线程
struct thread_info* thread_start(char* name,uint8_t priority,thread_func function,void* func_arg)
{

	struct thread_info* thread_info = get_kernel_pge(TASK_PAGE_SIZE);
	init_thread(thread_info, name, priority);
	// thread_create(thread_info, function, func_arg);

	// __asm__ __volatile__ ("movl %0, %%esp; \
	// pop %%ebp;pop %%ebx;pop %%edi;pop %%esi; \
	// ret": :"g" (thread_info->self_kstack) : "memory");
	// return thread_info;
	
	// 获取当前任务所属进程的 task_struct
	// struct task_struct* curr = get_runningthread_pcb()->group_leader;
	// 给所属进程的 task_struct->thread_group 加入当前节点

}


/*
 *	0xc009e000 ~ 0xc009f000是内核主线程的内核栈
 *	0xc009d000 ~ 0xc009e000内核主线程的pcb.		*/
void kernel_main_thread_init()
{
	// 这里得到的是 0xc009e000
	struct thread_info * kernel_stack = (struct thread_info *)get_runthread_kstack();

	init_thread(kernel_stack,"init",31);
}