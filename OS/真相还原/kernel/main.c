#include "print.h"
int main(void) {
    asm("xchg %bx,%bx");
    // char buff[16] = "I am kernel\n";
    put_str("I am kernel\n");
    put_char('A');
    put_int(9);
    put_char('\n');
    put_int(0x12345678);
    put_char('\n');
    put_int(0x00000000);
   while(1);
}
