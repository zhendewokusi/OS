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

include/linux/irqflags.h 



static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = &clockevent_davinci;

	evt->event_handler(evt);
	return IRQ_HANDLED;
}

----------------

测试task_struct的thread_group是否工作正常。
```shell
(gdb) p thread_info->task->thread_group->next
$6 = (struct list_head *) 0xc009d064
(gdb) p thread_info->task->thread_group->next->next
$7 = (struct list_head *) 0xc0106064
(gdb) p thread_info->task->thread_group->next->next->next
$8 = (struct list_head *) 0xc0108064
(gdb) p thread_info->task->thread_group->next->next->next->next
$9 = (struct list_head *) 0xc009d064
(gdb)
```


----------------------------------------------------------------



底半机制是操作系统内核中一种用于处理中断事件的机制。在计算机系统中，中断是一种由硬件或软件触发的事件，它需要立即处理以响应系统的需求或外部设备的请求。处理中断事件的过程需要尽快完成，以确保系统的及时响应和稳定性。

底半机制通常被用来分离中断处理的两个阶段，分别是快速响应和延迟处理。具体来说，底半机制将中断处理过程分为两个部分：

1. **顶半部分（Top Half）**：也称为中断服务程序（ISR），是中断处理的第一阶段，负责尽快响应中断并执行与中断相关的必要操作。顶半部分通常会尽快完成，并迅速返回中断处理程序，以便系统能够继续执行其他任务，从而减少中断延迟。

2. **底半部分（Bottom Half）**：是中断处理的第二阶段，负责执行与中断事件相关的长时间或延迟敏感的操作，例如处理大量数据、文件系统同步、内存回收等。底半部分的执行可以在相对较慢的时间段内完成，因此不会影响到对系统的实时响应。

底半机制的引入使得中断处理变得更加高效和灵活，因为它可以帮助内核区分中断处理的不同阶段，并根据需求将处理延迟敏感的操作移到稍后执行。这有助于提高系统的响应速度和整体性能。

内核从2.6版本开始更改定时器中断处理为软中断机制。

软中断机制：软中断机制允许内核在非中断上下文中执行中断处理的延迟部分。软中断是通过内核线程来实现的，可以在更灵活的时间内执行，而不受中断上下文的限制。这使得软中断能够执行更加复杂和长时间的任务，如定时器管理、网络数据包处理等。


----------------------------------------------------------------


```c
linux-2.6.25/arch/x86/kernel/time_32.c:78  
/*
 * This is the same as the above, except we _also_ save the current
 * Time Stamp Counter value at the time of the timer interrupt, so that
 * we later on can estimate the time of day more exactly.
 */
irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	/* Keep nmi watchdog up to date */
	per_cpu(irq_stat, smp_processor_id()).irq0_irqs++;

#ifdef CONFIG_X86_IO_APIC
	if (timer_ack) {
		/*
		 * Subtle, when I/O APICs are used we have to ack timer IRQ
		 * manually to reset the IRR bit for do_slow_gettimeoffset().
		 * This will also deassert NMI lines for the watchdog if run
		 * on an 82489DX-based system.
		 */
		spin_lock(&i8259A_lock);
		outb(0x0c, PIC_MASTER_OCW3);
		/* Ack the IRQ; AEOI will end it automatically. */
		inb(PIC_MASTER_POLL);
		spin_unlock(&i8259A_lock);
	}
#endif

	do_timer_interrupt_hook();

	if (MCA_bus) {
		/* The PS/2 uses level-triggered interrupts.  You can't
		turn them off, nor would you want to (any attempt to
		enable edge-triggered interrupts usually gets intercepted by a
		special hardware circuit).  Hence we have to acknowledge
		the timer interrupt.  Through some incredibly stupid
		design idea, the reset for IRQ 0 is done by setting the
		high bit of the PPI port B (0x61).  Note that some PS/2s,
		notably the 55SX, work fine if this is removed.  */

		u8 irq_v = inb_p( 0x61 );	/* read the current state */
		outb_p( irq_v|0x80, 0x61 );intr_timer_handler
}
```

----------------------------------------------------------------

jiffies及其溢出?

全局变量jiffies取值为自操作系统启动以来的时钟滴答的数目，在头文件<linux/sched.h>中定义，数据类型为unsigned long volatile (32位无符号长整型)。

jiffies转换为秒可采用公式：(jiffies/HZ)计算，

将秒转换为jiffies可采用公式：(seconds*HZ)计算。

当 时钟中断发生时，jiffies 值就加1。因此连续累加一年又四个多月后就会溢出(假定HZ=100，1个jiffies等于1/100秒，jiffies可记录的最大秒数为 (2^32 -1)/100=42949672.95秒，约合497天或1.38年)，即当取值到达最大值时继续加1，就变为了0。