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

#define  NICE_0_LOAD 0
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

struct sched_class {
    // 指向下一个调度类指针
    const struct sched_class *next;
    // 将一个新的任务加入到调度器的就绪队列中
    void (*enqueue_task) (struct rq *rq, struct task_struct *p, int wakeup);
    // 从调度器的就绪队列中移除一个任务
    void (*dequeue_task) (struct rq *rq, struct task_struct *p, int sleep);
    // 当前任务主动放弃 CPU 使用权，以便其他任务可以执行
    void (*yield_task) (struct rq *rq);
    //  检查当前任务是否应该被抢占
    void (*check_preempt_curr) (struct rq *rq, struct task_struct *p);
    // 选择下一个要运行的任务
    struct task_struct * (*pick_next_task) (struct rq *rq);
    // 将前一个任务从CPU中移除，更新前一个任务的调度信息
    void (*put_prev_task) (struct rq *rq, struct task_struct *p);
    // 设置当前正在运行的任务
    void (*set_curr_task) (struct rq *rq);
    // 每次时钟滴答时调用的函数，用于更新任务的运行时间和调度状态
    void (*task_tick) (struct rq *rq, struct task_struct *p, int queued);
    // 当新任务被创建时调用的函数，用于初始化新任务的调度信息
    void (*task_fork) (struct rq *rq, struct task_struct *p);
    // 当前任务切换到其他任务之前调用的函数，用于更新当前任务的调度信息
    void (*switched_from) (struct rq *this_rq, struct task_struct *task,
                           int running);
    // 其他任务切换到当前任务时调用的函数，用于更新当前任务的调度信息
    void (*switched_to) (struct rq *this_rq, struct task_struct *task,
                         int running);
    // 当任务的优先级发生变化时调用的函数，用于更新任务的调度信息
    void (*prio_changed) (struct rq *this_rq, struct task_struct *task,
                          int oldprio, int running);
    // 更新当前任务的调度信息。
    void (*update_curr)(struct cfs_rq *cfs_rq);
};

// 实现时间记账功能
//
static void update_curr(struct cfs_rq *cfs_rq);
void __sched schedule_init();
void __sched schedule();


#include "sched_fair.h"

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

// 这里只有单个处理器，不涉及多核负载均衡等，简单处理
static struct rq rq;
static struct cfs_rq cfs_rq;
#endif
/*
 *   根据当前系统的情况计算targeted latency（调度周期），在这个调度周期中计算当前进程应该获得的时间片（物理时间），然后计算当前进程已经累积执行的物理时间，如果大于当前应该获得的时间片，那么更新本进程的vruntime并标记need resched flag，并在最近的一个调度点发起调度。
 */