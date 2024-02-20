#include "traps.h"
#include "interrupt.h"
#include "print.h"

void kernelDebugInfo(const char * filename,const char * func,int line,const char * condition)
{
        // 关中断
        set_intr_status(intr_off);
        // 打印错误信息
        put_str("\n---ASSERT ERROR!---\n");
        put_str("filename:");put_str(filename);put_str("\n");
        put_str("function:");put_str(func);put_str("\n");
        put_str("line:");put_int(line);put_str("\n");
        put_str("condition:");put_str(condition);put_str("\n");
        while (1);
}