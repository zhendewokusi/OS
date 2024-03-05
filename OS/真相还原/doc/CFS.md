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

任务： `context_switch()`