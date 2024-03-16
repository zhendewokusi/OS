#include "stdint.h"
#include "kernel.h"
#include "thread.h"
#include "sched.h"
#include "sched_fair.h"

// 任务的最小抢占粒度，默认值为 750000 微秒(0.75ms)
unsigned int sysctl_sched_min_granularity		= 750000ULL;

// 6ms
unsigned int sysctl_sched_latency			= 6000000ULL;
static unsigned int sched_nr_latency = 8;

static inline void account_entity_enqueue(struct cfs_rq *cfs_rq,struct sched_entity* se)
{
        update_load_add(&cfs_rq->load,se->load.weight);
        cfs_rq->nr_running++;
        se->on_rq = 1;
}


static inline int64_t find_prio_index(uint64_t val)
{
        for(int i = 0;i < 40;++i) {
                if(sched_prio_to_weight[i] == val)
                        return i;
        }
        // 如果发生错误了，返回默认的 weight 值
        return 20;
}

static inline uint64_t max_vruntime(uint64_t min_vruntime, uint64_t vruntime)
{
        int64_t delta = (int64_t)(vruntime - min_vruntime);
        if (delta > 0)
                min_vruntime = vruntime;

        return min_vruntime;
}

static inline uint64_t min_vruntime(uint64_t min_vruntime, uint64_t vruntime)
{
        int64_t delta = (int64_t)(vruntime - min_vruntime);
        if (delta < 0)
                min_vruntime = vruntime;

        return min_vruntime;
}
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
 * 该宏 SRR 定义了一个名为 "Shift Right and Round" 的操作，其功能是对给定的整数 x 进行右移 y 位的同时进行四舍五入。
 * 这里的四舍五入是指在移位之前，先对 x 加上 (1UL << ((y) - 1)) 这个值，这样在右移时，最低位（即将被移出的位）若为1，则会触发进位，从而实现类似于浮点数除以2的向上（四舍五入）取整效果。
 * 举个例子，假设 x = 0b1011，y = 2，那么：
 * 先计算 1UL << ((y) - 1)，即 1UL << 1，结果为 0b10。
 * 把这个值加到 x 上：0b1011 + 0b10 = 0b1100。
 * 对结果进行右移 y 位：0b1100 >> 2，结果为 0b11。
 * 原本右移两位不四舍五入的话结果是 0b1，而现在通过 SRR 宏实现了四舍五入得到了 0b11，这对于需要保留整数部分并进行近似操作的场景非常有用。
 */
#define SRR(x, y) (((x) + (1UL << ((y) - 1))) >> (y))

/*
 * 函数作用：计算根据weight和当前cfs_rq的总load调整后的 delta
 * delta_exec : 当前调度周期长度
 * q_load:        当前 *_rq 中的 load（这里只实现了cfs_rq）
 * weight:      当前的sched_entity 的 load
 */
static unsigned long __calc_delta(uint64_t delta_exec,uint64_t weight,struct load_weight * q_load)
{
        #define WMULT_CONST     (1UL << 32)
        #define WMULT_SHIFT     32
        // 如果倒数为 0
        if(unlikely(!q_load->inv_weight))
                q_load->inv_weight = (WMULT_CONST - q_load->weight/2) / (q_load->weight + 1);
        uint64_t tmp = delta_exec * weight;

        if(unlikely(tmp > WMULT_CONST))
                // 将高位数据丢弃，保留较低32位
                tmp = SRR(SRR(tmp, WMULT_SHIFT/2) * q_load->inv_weight,
                          WMULT_SHIFT/2);
        else
                tmp = SRR(tmp * q_load->inv_weight, WMULT_SHIFT);

        return (unsigned long)min(tmp,(uint64_t)(unsigned long)LONG_MAX);
}

/*
 * 函数作用：根据任务的权重调整给定的时间增量
 */
static uint64_t calc_delta_fair(uint64_t delta,struct sched_entity* se)
{
        if(unlikely(se->load.weight != NICE_0_LOAD)){
                delta = __calc_delta(se->load,delta);
        }
        return delta;
}
/*
 * 函数作用：惩罚的时间计算函数
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
        } else {
                // 给刚唤醒的进程减去一定的虚拟时间作为补偿
                vruntime -= sysctl_sched_latency;
        }
        // 补偿但是不能让虚拟时间进行倒退，不然就有问题(奖励多了[不是])
        se->vruntime = max_vruntime(se->vruntime,vruntime);
}

static void task_fork_fair(struct rq * rq,struct task_struct *p)
{
        struct sched_entity* se = &p->se,*curr = cfs_rq.curr;
        // 获取 rq lock
        // ...
        __update_rq_clock(rq);

        if(curr) {
                // 更新当前正在运行的调度实体的运行时间信息
                update_curr(&cfs_rq);
                se->vruntime = curr->vruntime;
        }
        // 更新调度实体的 vruntime（和 cfs_rq.min_vruntime 相差不能太大，否则疯狂占用CPU）
        place_entity(&cfs_rq, se, 1);
        // 只留下相对虚拟时间，之后唤醒时会加上当前rq 的min_vruntime
        se->vruntime -= cfs_rq.min_vruntime;
        // 释放 rq lock
        // ...

}

// 函数作用： 更新 cfs 的 min_vruntime
static void update_min_vruntime(struct cfs_rq* cfs_rq)
{
        struct sched_entity* curr = cfs_rq->curr;
        struct rb_node* leftmost = cfs_rq->rb_leftmost;
        uint64_t vruntime = cfs_rq->min_vruntime;

        if(curr) {
                if(curr->on_rq)
                        vruntime = curr->vruntime;
                else
                        curr = NULL;
        }

        if(leftmost) {
                // 获取 leftmost 的 sched_entity
                struct sched_entity* se;
                se = rb_entry(leftmost,struct sched_entity,run_node);

                if(!curr)
                        vruntime = se->vruntime;
                else
                        vruntime = min_vruntime(vruntime,se->vruntime);
        }
        // 防止时钟回退
        cfs_rq->min_vruntime = max_vruntime(cfs_rq->min_vruntime,vruntime);
}




//static inline void
//__update_curr(struct cfs_rq *cfs_rq, struct sched_entity *curr,
//	      unsigned long delta_exec)
//{
//	 unsigned long delta_exec_weighted;      // 记录任务运行时间的加权值
//
//	 schedstat_set(curr->exec_max, max((u64)delta_exec, curr->exec_max));
//
//	 curr->sum_exec_runtime += delta_exec;
//	 schedstat_add(cfs_rq, exec_clock, delta_exec);
//	 delta_exec_weighted = delta_exec;
//	 if (unlikely(curr->load.weight != NICE_0_LOAD)) {
//	 	delta_exec_weighted = calc_delta_fair(delta_exec_weighted,
//	 						&curr->load);
//	 }
//	 curr->vruntime += delta_exec_weighted;
//}


// 更新当前正在运行的调度实体的运行时间信息
static void update_curr(struct cfs_rq *cfs_rq)
{
        struct sched_entity * curr = cfs_rq->curr;
        uint64_t now = rq_of(cfs_rq)->clock;

        if (unlikely(!curr))
                return;

        unsigned long delta_exec = (unsigned long)(now - cfs_rq->exec_start);
        // 异常，时钟回退
        if(unlikely(int64_t)delta_exec <= 0)
        return;

//         __update_curr(cfs_rq, curr,delta_exec);

        curr->exec_start = now;
        curr->sum_exec_runtime += delta_exec;
        curr->vruntime += calc_delta_fair(delta_exec,curr);

        cfs_rq->exec_clock += delta_exec;
        update_min_vruntime(cfs_rq);
}

static inline int entity_before(struct sched_entity *a, struct sched_entity *b)
{
        return (s64)(a->vruntime - b->vruntime) < 0;
}

static void __enqueue_entity(struct cfs_rq* cfs_rq,struct sched_entity* se)
{
        struct rb_node ** link = &cfs_rq->tasks_timeline.rb_node;
        struct rb_node *parent = NULL;
        struct sched_entity* entry = NULL;
        uint8_t left_most = 1;
        while(*link) {
                parent = *link;
                entry = rb_entry(parent,struct sched_entity,run_node);
                if(entity_before(se,entry)) {
                        link = &parent->rb_left;
                }else{
                        link = &parent->rb_right;
                        left_most = 0;
                }
        }
        // 保持 vruntime 单调递增
        if(left_most) {
                cfs_rq->rb_leftmost = &se->run_node;
                cfs_rq->min_vruntime =
                        max_vruntime(cfs_rq->min_vruntime,se->vruntime);
        }
        // 节点链接到树中
        rb_link_node(&se->run_node,parent,link);
        // 维护红黑树
        rb_insert_color(&se->run_node,&cfs_rq->tasks_timeline);
}

static void enqueue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se, int flags)
{
        // 更新调度信息
        update_curr(cfs_rq);
        // 之前task_fork_fair中减去的，现在加上
        se->vruntime += cfs_rq->min_vruntime;
        // 更新就绪队列相关信息
        account_entity_enqueue(cfs_rq,se);
        // 将其添加到红黑树上
        if(cfs_rq->curr == se)
                __enqueue_entity(cfs_rq,se);

}

static void enqueue_task_fair(struct rq *rq, struct task_struct *p, int flags)
{
        struct cfs_rq * cfs = &cfs_rq;
        struct sched_entity* se = &p->se;
        // 这里如果有开启组调用，就需要循环将组内每个entity都遍历
        enqueue_entity(cfs,se,1);
        rq->nr_running += 1;
}

static const struct  sched_class fair_sched_class = {
        next = &fair_sched_class,
        task_fork = &task_fork_fair,
        enqueue_task = enqueue_task_fair,
};