#ifndef __LIB_STDINT_H__
#define __LIB_STDINT_H__
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;
#define bool char
#define LONG_MAX	((long)(~0UL>>1))
#endif
