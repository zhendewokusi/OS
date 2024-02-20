#ifndef  __STRING_H__
#define  __STRING_H__
#include "stdint.h"
#include "global.h"

extern inline void * memset(void * s,char c,uint32_t count);
extern inline void * memcpy(void * s,const char * src,int n);
extern inline int memcmp(const void * s, const void * b , uint32_t count);

extern inline uint32_t strlen(const char *str);
extern inline char* strcpy(char* dst_, const char* src_);
extern inline int8_t strcmp (const char *a, const char *b); 
extern inline char* strchr(const char* string, const uint8_t ch);
extern inline char* strrchr(const char* string, const uint8_t ch);
extern inline char* strcat(char* dst_, const char* src_);
extern inline uint32_t strchrs(const char* filename, uint8_t ch);
#endif