#ifndef __TIMER_H__
#define __TIMER_H__

#include "stdint.h"

// 这里可能发生的溢出错误暂不考虑，可以设置几个宏来保证。
static uint64_t volatile jiffies_64;

void timer_init(void);

#endif