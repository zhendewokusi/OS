#include <string.h>
#include "interrupt.h"
#include "print.h"
#include "init.h"
#include "global.h"
#include "traps.h"
#include "string.h"

int main(void) {
        init_all();
        char* buff = "I am kernel\n";
        char* tmp = "123";
        
        while(1);
}
