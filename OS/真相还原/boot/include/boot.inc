    LOADER_BASE_ADDR equ 0x900
    LOADER_START_SECTOR equ 0x1
    
;; 159 页有问题
;; kernel_init函数的 p_type 有问题
;; 服了,ELF64和ELF32文件书里面编译的时候都不写-m32参数吗?用32位电脑写的书？
;; 自己gcc编译的32位置文件会有很多额外的程序头,程序有bug可能是因为这些头的Virtaddr,对应的分页还没给分配,目前只有低1MB和高1GB的映射关系,其他地方存储会报错的.....服了,debug好久最后一步步才发现这个问题
;; 新问题:如何查看描述符的DPL