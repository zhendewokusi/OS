#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "global.h"
#include "traps.h"

int main(void) {
        // char* buff = "I am kernel\n";
        // // put_str("I am kernel\n");
        // put_str(buff);
        // char buff2[] = "I am kernel\n";
        // put_str(buff2);
        // // char buff3[20] = "I am kernel\n";
        // // put_str(buff3);
        // // asm("int3");
        // put_char('A');
        // put_int(9);
        // put_char('\n');
        // put_int(0x12345678);
        // put_char('\n');
        // put_int(0x00000000);
        // put_char('\n');
        // __asm__ __volatile__("sti");	     // 演示中断处理
        init_all();
        // __STI();
        // enum intr_status tmp1 = get_intr_status();
        // __CLI();
        // __MAGIC_BREAK();
        // enum intr_status tmp3 = set_intr_status(intr_off);
        // enum intr_status tmp2 = get_intr_status();
        // put_int(tmp1);
        // put_char('\n');
        // put_int(tmp2);
        // put_char('\n');
        // put_int(tmp3);
        // put_char('\n');
        ASSERT(1==2);
        while(1);
}
