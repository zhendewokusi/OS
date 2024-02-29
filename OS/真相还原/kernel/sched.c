#include "sched.h"
#include <bits/types/struct_sched_param.h>
#include <cstdint>

// 根据当前进程数量计算加权值并且加到vruntime
static void __update_curr(struct cfs_rq *cfs_rq, struct sched_entity *curr,
	      unsigned long delta_exec)
{

}

static void update_curr(struct cfs_rq *cfs_rq)
{
        struct sched_entity * curr = cfs_rq->curr;
        // 获取 rq 中的 clock
        uint64_t now = func();
        // 判断任务是否为空
        // 32位下不会溢出
        unsigned long delta_exec = (unsigned long)(now - cfs_rq->exec_start);
        __update_curr(cfs_rq, curr,delta_exec);
        curr->exec_start = now;
        // 判断是当前进程是否是任务，如果是则将设置一个指向task_struct的指针curtask
        // 更新当前进程 curtask 的 CPU 使用情况统计信息
}
