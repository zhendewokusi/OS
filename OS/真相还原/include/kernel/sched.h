#ifndef __SCHED_H__
#define __SCHED_H__

#include <cstdint>
#include "stdint.h"
#include "linux/rbtree.h"

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
	// runqueue 锁
	uint64_t clock;
};

// 实现时间记账功能
static void update_curr(struct cfs_rq *cfs_rq);

#endif