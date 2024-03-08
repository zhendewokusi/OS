#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
#include "stdint.h"
typedef void* intr_handler;
// 初始化中断
void idt_init(void);
// 注册中断处理函数
void register_handler(uint8_t vector,intr_handler function);

// 定义中断目前状态
enum intr_status {
        intr_off = 0,
        intr_on = 1
};

extern enum intr_status get_intr_status(void);
extern enum intr_status set_intr_status(enum intr_status status);



#endif
