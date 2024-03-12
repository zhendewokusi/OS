#include "linux/kernel.h"
#include "stdint.h"
#include "thread.h"
#include "timer.h"
#include "sched.h"



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

// 主要调度器函数
void __sched schedule()
{
	struct task_struct * prev,* next;
	unsigned long *switch_count;

need_resched:
	inc_preempt_count();		// 禁止抢占
	// 由于该OS是单处理器，就在sched.h直接定义了全局的 rq 和 cfs_rq
	prev = rq.curr;
	switch_count = &prev->nivcsw;
	
	// 释放先前进程的锁
need_resched_nonpreemptible:

	return;
}

void __sched schedule_init()
{
	INIT_STRUCT_RQ(&rq, &cfs_rq);
}
