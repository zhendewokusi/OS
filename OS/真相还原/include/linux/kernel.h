#ifndef __KERNEL_H__
#define __KERNEL_H__

// 根据结构体成员查找结构体地址
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

// 用于编译器优化
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

// 调度器相关的宏
/*
	如果需要确定系统的全局时钟间隔和调度器的行为，通常会使用 PIT_TICK_RATE；而如果需要与 PIT 相关的输入频率进行计算，则会使用 INPUT_FREQUENCY。
*/
#define PIT_TICK_RATE 1193182 /* Underlying HZ */
#define CLOCK_TICK_RATE	PIT_TICK_RATE

// 计算
#define LATCH  ((CLOCK_TICK_RATE + HZ/2) / HZ)	/* For divider */

/* Suppose we want to devide two numbers NOM and DEN: NOM/DEN, then we can
 * improve accuracy by shifting LSH bits, hence calculating:
 *     (NOM << LSH) / DEN
 * This however means trouble for large NOM, because (NOM << LSH) may no
 * longer fit in 32 bits. The following way of calculating this gives us
 * some slack, under the following conditions:
 *   - (NOM / DEN) fits in (32 - LSH) bits.
 *   - (NOM % DEN) fits in (32 - LSH) bits.
 */
#define SH_DIV(NOM,DEN,LSH) (   (((NOM) / (DEN)) << (LSH))              \
                             + ((((NOM) % (DEN)) << (LSH)) + (DEN) / 2) / (DEN))

// 计算实际的系统时钟频率
#define ACTHZ (SH_DIV (CLOCK_TICK_RATE, LATCH, 8))

/* TICK_NSEC is the time between ticks in nsec assuming real ACTHZ */
#define TICK_NSEC (SH_DIV (1000000UL * 1000, ACTHZ, 8))


// 建议编译器使用堆栈传参数而不用寄存器传参
#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))
// 建议编译器及时没有被使用，也不要优化删除
# define __used			__attribute__((__used__))
// 位运算
#define test_thread_flag(flag) \
	test_ti_thread_flag(current_thread_info(), flag)

#endif