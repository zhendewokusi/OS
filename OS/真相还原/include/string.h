#ifndef  __STRING_H__
#define  __STRING_H__
#include "stdint.h"
#include "descriptor.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

extern  void * memset(void * s,char c,uint32_t count);
extern  void * memcpy(void * s,const char * src,int n);
extern  int memcmp(const void * s, const void * b , uint32_t count);
extern  uint32_t strlen(const char *str);
extern  char* strcpy(char* dst_, const char* src_);
extern  int8_t strcmp (const char *a, const char *b); 
extern  char* strchr(const char* string, const uint8_t ch);
extern  char* strrchr(const char* string, const uint8_t ch);
extern  char* strcat(char* dst_, const char* src_);
extern  uint32_t strchrs(const char* filename, uint8_t ch);
#endif 