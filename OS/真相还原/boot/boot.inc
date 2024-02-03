    LOADER_BASE_ADDR equ 0x900
    LOADER_START_SECTOR equ 0x1
    
;; 159 页有问题
;; kernel_init函数的 p_type 有问题
;; 服了,ELF64和ELF32文件书里面编译的时候都不写-m32参数吗?