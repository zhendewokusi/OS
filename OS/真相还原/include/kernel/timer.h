#ifndef __TIMER_H__
#define __TIMER_H__

#include "stdint.h"

static uint64_t volatile jiffies_64;

void timer_init(void);

#endif