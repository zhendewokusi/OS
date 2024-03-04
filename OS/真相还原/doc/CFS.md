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