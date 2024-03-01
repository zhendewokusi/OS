#ifndef __SCHED_H__
#define __SCHED_H__

#include "stdint.h"
#include "linux/rbtree.h"

# define schedstat_inc(rq, field)	do { (rq)->field++; } while (0)
# define schedstat_add(rq, field, amt)	do { (rq)->field += (amt); } while (0)
# define schedstat_set(var, val)	do { var = (val); } while (0)

struct load_weight {
	unsigned long weight, inv_weight;
};

struct sched_entity {
	struct load_weight	load;		/* for load-balancing */
	struct rb_node		run_node;
	unsigned int		on_rq;
	uint64_t		exec_start;
	uint64_t		sum_exec_runtime;
	uint64_t		vruntime;
	uint64_t		prev_sum_exec_runtime;
	uint64_t		last_wakeup;
	uint64_t		avg_overlap;
};

struct cfs_rq {
	struct load_weight load;
	unsigned long nr_running;

	uint64_t exec_clock;
	uint64_t min_vruntime;

	struct rb_root tasks_timeline;
	struct rb_node *rb_leftmost;
	struct rb_node *rb_load_balance_curr;

	struct sched_entity *curr, *next;

	unsigned long nr_spread_over;
};

struct rq {
	// runqueue lock
	// ...
	struct load_weight load;

	// 一个记录实际的物理时间
	//一个记录task运行的时间，这里如果发生中断等时间需要停止加
	uint64_t clock;
	
	struct cfs_rq cfs;
};

// 实现时间记账功能
static void update_curr(struct cfs_rq *cfs_rq);

#endif

/*

根据当前系统的情况计算targeted latency（调度周期），在这个调度周期中计算当前进程应该获得的时间片（物理时间），然后计算当前进程已经累积执行的物理时间，如果大于当前应该获得的时间片，那么更新本进程的vruntime并标记need resched flag，并在最近的一个调度点发起调度。

 */