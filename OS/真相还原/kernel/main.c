#include "stdint.h"
#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "descriptor.h"
#include "system.h"
#include "traps.h"
#include "string.h"


uint64_t mem_size = 0x100000000;

int main(void) {
        
        // 初始化
        init_all();

        while(1);
}
