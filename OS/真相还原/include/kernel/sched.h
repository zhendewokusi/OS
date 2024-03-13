#ifndef __SCHED_H__
#define __SCHED_H__

#include "memory.h"
#include "stdint.h"
#include "linux/rbtree.h"
#include "system.h"
// #include "thread.h"

// 单独将调度器核心代码放置在一个段中，提高缓存命中,便于代码优化
#define __sched __attribute__((__section__(".sched.text")))

# define schedstat_inc(rq, field)	do { (rq)->field++; } while (0)
# define schedstat_add(rq, field, amt)	do { (rq)->field += (amt); } while (0)
# define schedstat_set(var, val)	do { var = (val); } while (0)

#define curr_preempt_count() current_thread_info()->preempt_count

#define add_preempt_count(val) curr_preempt_count() += (val)
#define inc_preempt_count() add_preempt_count(1)

#define sub_preempt_count(val) curr_preempt_count() -= (val)
#define del_preempt_count() sub_preempt_count(1)

// 默认负载权重
#define SCHED_LOAD_SCALE	(1L << 10)



struct sched_entity {
	uint64_t		load;
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
	uint64_t load;
	unsigned long nr_running;

	uint64_t exec_clock;
	uint64_t min_vruntime;

	struct rb_root tasks_timeline;
	struct rb_node *rb_leftmost;

	struct sched_entity *curr, *next;
};

struct rq {
	// runqueue lock
	// ...
	uint64_t load;

	// clock 表示当前CPU 运行队列的时钟（CPU运行时间）
	// prev_clock_raw 记录上一个
	uint64_t clock,prev_clock_raw;
	uint64_t clock_warps;
	uint64_t tick_timestamp;
	uint64_t clock_max_delta;
	struct task_struct *curr, *idle;	// 后者是空闲进程，当没有任务时候调用该进程（节能）
	struct cfs_rq* cfs;
};

// 这里只有单个处理器，不涉及多核负载均衡等，简单处理
static struct rq rq;
static struct cfs_rq cfs_rq;

// 实现时间记账功能



#define INIT_STRUCT_CFSRQ(cfs_addr) do {  \
    (cfs_addr)->load = SCHED_LOAD_SCALE;   \
    (cfs_addr)->nr_running = 0;            \
    (cfs_addr)->exec_clock = 0;            \
    (cfs_addr)->min_vruntime = 0;          \
    (cfs_addr)->tasks_timeline = RB_ROOT;     \
    (cfs_addr)->rb_leftmost = NULL;        \
    (cfs_addr)->curr = current_thread_info(); \
    (cfs_addr)->next = NULL;               \
} while(0)
// (cfs_addr)->tasks_timeline = NULL;	这里需要写一个初始化红黑树的函数

#define INIT_STRUCT_RQ(rq_addr,cfs_addr)	\
    do{	(rq_addr)->load  = 0;			\
	(rq_addr)->clock = 0;			\
	(rq_addr)->prev_clock_raw = 0;		\
	(rq_addr)->clock_warps = 0;		\
	(rq_addr)->tick_timestamp = 0;		\
	(rq_addr)->clock_max_delta = 0;		\
	(rq_addr)->curr = current_thread_info();\
	(rq_addr)->idle = NULL;			\
	(rq_addr)->cfs = cfs_addr;		\
	INIT_STRUCT_CFSRQ(cfs_addr);		\
} while(0)

#endif

static void update_curr(struct cfs_rq *cfs_rq);
void __sched schedule_init();
void __sched schedule();

/*

根据当前系统的情况计算targeted latency（调度周期），在这个调度周期中计算当前进程应该获得的时间片（物理时间），然后计算当前进程已经累积执行的物理时间，如果大于当前应该获得的时间片，那么更新本进程的vruntime并标记need resched flag，并在最近的一个调度点发起调度。

 */