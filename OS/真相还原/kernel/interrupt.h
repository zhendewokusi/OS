#ifndef __INTERRUPT_H
#define __INTERRUPT_H
// #include "stdint.h"
typedef void* intr_handler;
// 初始化中断
void idt_init(void);

// 定义中断目前状态
enum intr_status {
    intr_off = 0,
    intr_on = 1
};

enum intr_status get_intr_status(void);



#endif
