#include "print.h"
int main(void) {
    char* buff = "I am kernel\n";
    // put_str("I am kernel\n");
    put_str(buff);
    char buff2[] = "I am kernel\n";
    put_str(buff2);

    // char buff3[20] = "I am kernel\n";
    // put_str(buff3);
    // asm("int3");
    put_char('A');
    put_int(9);
    put_char('\n');
    asm("xchg %bx,%bx");
    put_int(0x12345678);
    put_char('\n');
    put_int(0x00000000);
   while(1);
}
