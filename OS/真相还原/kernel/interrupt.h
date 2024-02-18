#ifndef __INTERRUPT_H
#define __INTERRUPT_H
// #include "stdint.h"
typedef void* intr_handler;
void idt_init(void);
#endif
