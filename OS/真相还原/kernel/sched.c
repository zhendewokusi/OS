#include "linux/kernel.h"
#include "stdint.h"
#include "thread.h"
#include "timer.h"
#include "sched.h"
#include "traps.h"


// static inline struct rq *rq_of(struct cfs_rq *cfs_rq)
// {
// 	return container_of(cfs_rq, struct rq, cfs);
// }
// 根据当前进程数量计算加权值并且加到vruntime
static inline void
__update_curr(struct cfs_rq *cfs_rq, struct sched_entity *curr,
	      unsigned long delta_exec)
{
	// unsigned long delta_exec_weighted;      // 记录任务运行时间的加权值

	// schedstat_set(curr->exec_max, max((u64)delta_exec, curr->exec_max));

	// curr->sum_exec_runtime += delta_exec;
	// schedstat_add(cfs_rq, exec_clock, delta_exec);
	// delta_exec_weighted = delta_exec;
	// if (unlikely(curr->load.weight != NICE_0_LOAD)) {
	// 	delta_exec_weighted = calc_delta_fair(delta_exec_weighted,
	// 						&curr->load);
	// }
	// curr->vruntime += delta_exec_weighted;
}

// 计算当前进程执行时间并且存储在 delta_exec 中
static void update_curr(struct cfs_rq *cfs_rq)
{
        // struct sched_entity * curr = cfs_rq->curr;
        // // 获取 rq 中的 clock
        // uint64_t now = rq_of(cfs_rq)->clock;
        // // 判断任务是否为空
        // if (unlikely(!curr))
        //         return;
        // // 32位下不会溢出
        // unsigned long delta_exec = (unsigned long)(now - cfs_rq->exec_start);
        // __update_curr(cfs_rq, curr,delta_exec);
        // curr->exec_start = now;
        // 判断是当前进程是否是任务，如果是则将设置一个指向task_struct的指针curtask
        
        // 更新当前进程 curtask 的 CPU 使用情况统计信息（用于调度算法)
}

// 此处没有高精度计时器，使用节拍数来记录
static void __update_rq_clock(struct rq * rq)
{
	uint64_t prev_raw = rq->prev_clock_raw;
	uint64_t now = JIFFIES_TO_NSEC(jiffies_64);
	int64_t delta = now - prev_raw;
	uint64_t clock = rq->clock;

	// 发生时钟回退
	if(unlikely(delta < 0)) {
		clock++;
		rq->clock_warps++;
	} else {
		// 如果发生了太大的时间跳跃
		if (unlikely(clock + delta > rq->tick_timestamp + TICK_NSEC)) {
			if (clock < rq->tick_timestamp + TICK_NSEC)
				clock = rq->tick_timestamp + TICK_NSEC;
			else
				clock++;
			rq->clock_overflows++;
		} else {
			// 更新 clock 和 clock_max_delta
			if (unlikely(delta > rq->clock_max_delta))
				rq->clock_max_delta = delta;
			clock += delta;
		}
	}
	rq->prev_clock_raw = now;
	rq->clock = clock;
}


// 主要调度器函数
void __sched schedule()
{
	struct task_struct * prev,* next;
	unsigned long *switch_count;
	struct rq *now_rq;
need_resched:
	now_rq = &rq;			// 只有一个CPU,简单处理
	inc_preempt_count();		// 禁止抢占
	prev = now_rq->curr;
	switch_count = &prev->nivcsw;

need_resched_nonpreemptible:
	__CLI();			// 禁止本地中断
	__update_rq_clock();		// 更新rq的时钟
        // 搶 rq 鎖
        clear_task_need_resched(prev);  // 清除需要搶佔的標誌
//        if(prev->)
	return;
}

//void __sched schedule_init() {
//	INIT_STRUCT_RQ(&rq, &cfs_rq);
//}

/*
 * p 指向即将被创建的子进程的 task_struct 结构体。
 */
int schedule_fork(unsigned long clone_flags,struct  task_struct * p)
{
    unsigned long tmp = clone_flags;
    tmp++;
    p->priority = current()->priority;
    p->sched_class = &fair_sched_class; // 这里使用 CFS 调度类
    if(p->sched_class.task_fork)
        p->sched_class.task_fork(p);
    return 0;
}