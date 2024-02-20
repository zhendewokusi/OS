#include "string.h"
#include "traps.h"

extern inline void *memset(void *s, char c, uint32_t count) {
        ASSERT(s != NULL);
        __asm__ __volatile__(
        "cld\n\t"
        "rep\n\t"
        "stosb"
        :
        : "a" (c), "D" (s), "c" (count)
        : "memory"
        );
        return s;
}

extern inline void *memcpy(void *dst,const char * src,int n) {
        ASSERT(dst != NULL && src != NULL);
        uint8_t* dst_ = dst;
        const uint8_t* src_ = src;
        while (n-- > 0)
                *dst_++ = *src_++;
}

extern inline int memcmp(const void *s, const void *b, uint32_t count) {
    int __res = 0;
    __asm__ __volatile__ (
        "cld\n\t"
        "repe\n\t"
        "cmpsb\n\t"
        "je 1f\n\t"
        "movl $1,%%eax\n\t"
        "jl 1f\n\t"
        "negl %%eax\n"
        "1:"
        : "=a" (__res)
        : "0" (0), "D" (s), "S" (b), "c" (count)
        : "memory"
    );
    return __res;
}

extern inline uint32_t strlen(const char *str)
{
        ASSERT(str != NULL);
        const char* p = str;
        while(*p++);
        return (p - str - 1);
}

extern inline char* strcpy(char* dst_, const char* src_)
{
        ASSERT(dst_ != NULL && src_ != NULL);
        char* r = dst_;		       // 用来返回目的字符串起始地址
        while((*dst_++ = *src_++));
        return r;
}

// 比较两个字符串,若a_中的字符大于b_中的字符返回1,相等时返回0,否则返回-1.
extern inline int8_t strcmp (const char* a, const char* b)
{
        ASSERT(a != NULL && b != NULL);
        while (*a != 0 && *a == *b) {
                a++;
                b++;
        }
        return *a < *b ? -1 : *a > *b;
}

// 从左到右查找字符串str中首次出现字符ch的地址(不是下标,是地址)
// 需要强制转化成和返回值类型一样,否则编译器会报const属性丢失
extern inline char* strchr(const char* str, const uint8_t ch)
{
        ASSERT(str != NULL);
        while (*str != 0) {
                if (*str == ch) {
                return (char*)str;
                }
                str++;
        }
        return NULL;
}

// 从后往前查找字符串str中首次出现字符ch的地址(不是下标,是地址)
extern inline char* strrchr(const char* str, const uint8_t ch)
{
        ASSERT(str != NULL);
        const char* last_char = NULL;
        while (*str != 0) {
                if (*str == ch) {
                last_char = str;
                }
                str++;
        }
        return (char*)last_char;
}

// 将字符串src_拼接到dst_后,将回拼接的串地址
extern inline char* strcat(char* dst_, const char* src_)
{
        ASSERT(dst_ != NULL && src_ != NULL);
        char* str = dst_;
        while (*str++);
        str--;
        while((*str++ = *src_++));	 // 当*str被赋值为0时,此时表达式不成立,正好添加了字符串结尾的0.
        return dst_;
}

// 在字符串str中查找指定字符ch出现的次数
extern inline uint32_t strchrs(const char* str, uint8_t ch)
{
        ASSERT(str != NULL);
        uint32_t ch_cnt = 0;
        const char* p = str;
        while(*p != 0) {
                if (*p == ch) {
                        ch_cnt++;
                }
                p++;
        }
        return ch_cnt;
}