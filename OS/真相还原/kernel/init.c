#include "print.h"
#include "timer.h"
#include "init.h"
#include "interrupt.h"
#include "memory.h"
void init_all() {
        put_str("init_all\n");
        idt_init();
        timer_init();
        memo_init();
        return;
}