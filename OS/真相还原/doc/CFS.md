1. 更新`struct rq`内部的clock是如何做到的？

文件:kernel/sched.c:__update_rq_clock(struct rq *rq)

`rq->clock`存储的是队列的时钟值，每次更新是两次时间戳的差值。这里会有两个问题：时钟跳跃和时钟溢出。最终目的：保证系统的时间计算和调度的正确性。
时钟跳跃：检测到系统时钟向后跳跃。处理：队列时钟值+1,并且增加时钟跳跃计时器。保证队列时钟连续增长。
时钟溢出：检测到时钟增量超过某个阈值（clock + delta > rq->tick_timestamp【用于记录上次时钟滴答的时间戳】+ TICK_NSEC）。则需要修正运行队列的时钟值，使其不超过下一个时钟滴答的时间戳，并增加时钟溢出计数器。这样可以防止时钟值超出范围而导致的不连续性和错误。

__update_rq_clock ----> sched_clock()

```c
unsigned long long __attribute__((weak)) sched_clock(void)
{
        // jiffies 值是一个全局的、自增的计数器，用于跟踪系统的运行时间。发生一次时钟中断就 +1
        // NSEC_PER_SEC 表示每秒的纳秒数
        // HZ 表示内核中的时钟频率
	return (unsigned long long)jiffies * (NSEC_PER_SEC / HZ);
}
```

2. 何时进行调度？？
schedule(): line 3896

不能依靠用户显性调用`schedule()`，用户不调用不久一直执行它了....
内核提供了一个`need_resched`标志（在thread_info结构体中）。

那么何时进行标志？当进程应该被抢占时，`schedule_tick()`设置；当优先级高的进程进入可执行状态时，`try_to_wake_up()`设置标志。内核检查标志，如果确认其被设置，调用`schedule()`。

何时检查？内核在中断处理程序返回*用户空间*或者系统调用返回*用户空间*都会进行检查`need_resched`标志

3. 内核抢占？

Linux支持完整的内核抢占。什么时候调度是安全的？没有持有锁的时候。为了支持内核抢占，`thread_info`中加入了`preempt_count`使用锁+1,释放锁-1。
```c
#define preempt_disable() \
do { \
	inc_preempt_count(); \
	barrier(); \
} while (0)
```

发生时刻：

中断发生时候，且没有返回内核，任务阻塞，显式调用`schedule()`，内核代码具有可抢占性。

3. 上下文切换如何做？

`context_switch()`负责，`schedule()`调用此函数。
功能：
  - 虚拟内存从上一个进程更换到新进程
  - 调用`switch_to()`，该函数负责上一个进程的处理器状态切换到新进程的处理器状态

4. current?current_thread_info?
```c
#define percpu_from_op(op,var)					\
	({							\
		typeof(var) ret__;				\
		switch (sizeof(var)) {				\
		case 1:						\
			asm(op "b "__percpu_seg"%1,%0"		\
			    : "=r" (ret__)			\
			    : "m" (var));			\
			break;					\
		case 2:						\
			asm(op "w "__percpu_seg"%1,%0"		\
			    : "=r" (ret__)			\
			    : "m" (var));			\
			break;					\
		case 4:						\
			asm(op "l "__percpu_seg"%1,%0"		\
			    : "=r" (ret__)			\
			    : "m" (var));			\
			break;					\
		default: __bad_percpu_size();			\
		}						\
		ret__; })

#define x86_read_percpu(var) percpu_from_op("mov", per_cpu__##var)


static __always_inline struct task_struct *get_current(void)
{
	return x86_read_percpu(current_task);
}
#define current get_current()
```

在典型的 Linux 内核实现中，thread_info 结构体通常位于当前线程栈帧所在页内存的最低地址。然而目前是把 task_struct 放在最低下，emmmm，得改

```c
/* how to get the thread information struct from C */
static inline struct thread_info *current_thread_info(void)
{
	return (struct thread_info *)(current_stack_pointer & ~(THREAD_SIZE - 1));
}
```

很有意思一个点：为什么asm-alpha架构中pcb 和 task_struct 是分开的?

在 Alpha 架构中，`pcb`（进程控制块）和 `task_struct` 是分开的，这是因为 Alpha 架构上的 Linux 内核采用了不同的内存布局和进程管理设计。

1. **Alpha 架构上的内存布局**：
   在 Alpha 架构上，进程控制块（PCB）通常被称为进程描述符（process descriptor），存储在进程管理单元（Process Management Unit，PMU）中。PMU 是 Alpha 架构中专门用于管理进程信息的硬件单元。这意味着 PCB 存储在专门的硬件结构中，而不是像其他架构上的 Linux 内核一样存储在内核堆中。

2. **task_struct 的设计**：
   在 Alpha 架构上，`task_struct` 结构体仍然存在，但它主要用于 Linux 内核的内部管理和调度，而不是存储进程的全部信息。在 Alpha 架构上，`task_struct` 可能会更轻量化，只包含必要的调度和管理信息。因此，Alpha 架构上的 `task_struct` 不是进程控制块（PCB）的完整替代，而是 PCB 的一部分。

综上所述，Alpha 架构上的 PCB 和 `task_struct` 是分开的，因为它们分别在硬件级别和内核级别进行了不同的设计和实现，以适应 Alpha 架构的特定特性和需求。

在x86架构上，`pcb`（进程控制块）和 `task_struct` 是同一个概念，它们指代的是相同的数据结构。在Linux内核中，特别是在x86架构上，`task_struct` 结构体用于存储进程的所有信息，包括进程状态、调度信息、文件描述符等等。因此，`task_struct` 结构体即是进程的控制块，也是进程的描述符。

简而言之，在x86架构上，`pcb` 和 `task_struct` 并不是分开的，它们代表了相同的概念，即进程的控制块或进程描述符。

任务:`schedule_debug(struct task_struct *prev)`
函数作用：
 1. 检查是否处于原子上下文
 2. 记录调度次数
 3. 调度性能分析