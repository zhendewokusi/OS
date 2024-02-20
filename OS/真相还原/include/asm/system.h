#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#define __NOP() __asm__ __volatile__("nop")
#define __STI() __asm__ __volatile__("sti")
#define __CLI() __asm__ __volatile__("cli")
#define __MAGIC_BREAK() __asm__ __volatile__("xchg %bx,%bx")
#define __IRET() __asm__ __volatile__ ("iret"::)

#endif