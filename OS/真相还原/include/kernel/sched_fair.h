#ifndef __SCHED_FAIR_H__
#define __SCHED_FAIR_H__

struct sched_entity {
    uint64_t		load;
    struct rb_node	run_node;
    unsigned int	on_rq;
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

const int sched_prio_to_weight[40] = {
        /* -20 */     88761,     71755,     56483,     46273,     36291,
        /* -15 */     29154,     23254,     18705,     14949,     11916,
        /* -10 */      9548,      7620,      6100,      4904,      3906,
        /*  -5 */      3121,      2501,      1991,      1586,      1277,
        /*   0 */      1024,       820,       655,       526,       423,
        /*   5 */       335,       272,       215,       172,       137,
        /*  10 */       110,        87,        70,        56,        45,
        /*  15 */        36,        29,        23,        18,        15,
};

#endif
