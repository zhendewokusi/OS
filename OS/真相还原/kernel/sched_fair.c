#include "thread.h"
#include "sched.h"
#include "sched_fair.h"

// 任务的最小抢占粒度，默认值为 750000 微秒(0.75ms)
unsigned int sysctl_sched_min_granularity		= 750000ULL;

// 6ms
unsigned int sysctl_sched_latency			= 6000000ULL;
static unsigned int sched_nr_latency = 8;
/*
 * 函数作用： 找到并返回红黑树中的第一个公平调度实体对应的红黑树节点
 */
static inline struct rb_node *first_fair(struct cfs_rq * cfs_rq)
{
        return cfs_rq->rb_leftmost;
}

/*
 * 函数作用：更新最小虚拟运行时间
 */
static inline uint64_t min_vruntime(uint64_t min_vruntime, uint64_t vruntime)
{
        int64_t delta = (int64_t)(vruntime - min_vruntime);
        if (delta < 0)
                min_vruntime = vruntime;
        return min_vruntime;
}

/*
 * 函数作用：根据当前就绪队列中的任务数量来动态调整CFS调度器中的调度周期
 */
static uint64_t __sched_period(unsigned long nr_running)
{
        if(unlikely(nr_running > sched_nr_latency))
                return nr_running * sysctl_sched_min_granularity;
        else
                return sysctl_sched_latency;
}

/*
 * 函数作用：计算任务的基本时间片
 * 1. 计算调度周期,调度周期是用来计算基本时间片的基础
 * 2. 计算每个实体的基本时间片并且相加作为结果返回
 *
 * 基本时间片 = 调度周期 *（进程权重 / 就绪队列所有进程权重之和）
 */
static uint64_t sched_slice(struct cfs_rq* cfs_rq,struct sched_entity* se)
{
        uint64_t slice = __sched_period(cfs_rq->nr_running + !se->on_rq);
}
/*
 * 函数作用：
 */
static  uint64_t  __calc_delta(struct sched_entity *se,uint64_t delta)
{

}

/*
 * 函数作用：根据任务的权重调整给定的时间片增量
 */
static uint64_t calc_delta_fair(uint64_t delta,struct sched_entity* se)
{
        if(unlikely(se->load != NICE_0_LOAD)){
                delta = __calc_delta(se,delta);
        }
        return delta;
}
/*
 * 函数作用：计算要插入的任务的虚拟运行时间片
 * 虚拟运行时间片 = 基本时间片 / 任务权重
 */
static inline uint64_t sched_vslice(struct cfs_rq* cfs_rq,struct sched_entity* se)
{
        return calc_delta_fair(sched_slice(cfs_rq, se), se);
}

/*
 * 函数作用： 为新添加或唤醒的调度实体计算并设置其虚拟运行时间
 * 创建时候 initial == 1
 * 1. 创建或者更新调度实体得到虚拟时间
 * 2. 得到的时间要和cfs_rq->min_vruntime的值保持差别不大
 */
static void place_entity(struct cfs_rq* cfs_rq,struct sched_entity* se,int initial)
{
        uint64_t vruntime = cfs_rq->min_vruntime;
        // 这里默认开启，刚开始调度的时候会对其进行“记账”
        if(initial) {
                vruntime += sched_vslice(cfs_rq,se);
        }
}

static void task_new_fair(struct rq * rq, struct task_struct *p)
{
        struct sched_entity* se = &p->se,*curr = cfs_rq.curr;
        // 获取 rq lock
        // ...
        if(curr) {
                // 更新当前正在运行的调度实体的运行时间信息
                update_curr(&cfs_rq);
                se->vruntime = curr->vruntime;
        }
        // 更新调度实体的 vruntime（和 cfs_rq.min_vruntime 相差不能太大，否则疯狂占用CPU）
        place_entity(&cfs_rq, se, 1);

}

static const struct  sched_class fair_sched_class = {
        next = &fair_sched_class,
        task_fork = &task_fork_fair,
};