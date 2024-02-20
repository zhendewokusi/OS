#ifndef __TRAPS_H__
#define __TRAPS_H__
#include "descriptor.h"
#include "stdint.h"

void kernelDebugInfo(const char * filename,const char * func,int line,const char * condition);

#define __KERNEL_DEBUG_INFO__(...) kernelDebugInfo(__FILE__,__FUNCTION__,__LINE__,__VA_ARGS__)


#ifdef __NO_DEBUG__
        #define ASSERT(CONDITION)  __NOP();
#else
        #define ASSERT(CONDITION)       \
                if(!(CONDITION)) { __KERNEL_DEBUG_INFO__(#CONDITION); }
#endif

#endif