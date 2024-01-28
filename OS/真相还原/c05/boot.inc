    LOADER_BASE_ADDR equ 0x100
    LOADER_START_SECTOR equ 0x1
; ;段描述符中一些属性
; ; 高32位 :
;     ; 23位(G) : 粒度 
;     DESC_G_4K equ  1_0000_0000_0000_0000_0000_000b
;     DESC_G_1  equ  0_0000_0000_0000_0000_0000_000b
;     ; 22位(D/B) : 操作数 (命令,内存,栈)
;     DESC_D_32 equ   1_0000_0000_0000_0000_0000_00b
;     DESC_D_16 equ   0_0000_0000_0000_0000_0000_00b
;     ; 21位(L) : 是否是64位代码段
;     DESC_L_32    equ    0_0000_0000_0000_0000_0000_0b
;     DESC_L_64    equ    1_0000_0000_0000_0000_0000_0b
;     ; 20位(AVL) : OS使用,与硬件无关
;     DESC_AVL  equ     0_0000_0000_0000_0000_0000b 
;     ; 16~19位(段界限) : 16到19位的段界限
;     DESC_LIMIT_CODE_HIGH  equ 1111_0000_0000_0000_0000b
;     DESC_LIMIT_DATA_HIGH  equ DESC_LIMIT_CODE_HIGH
;     ; 15位(P) : 存在在内存
;     DESC_P equ  1_000000000000000b
;     ; 13~14位置(DPL) : 特权级
;     DESC_DPL_0 equ 00_0000_0000_00000b
;     DESC_DPL_1 equ 01_0000_0000_00000b
;     DESC_DPL_2 equ 10_0000_0000_00000b
;     DESC_DPL_3 equ 11_0000_0000_00000b
;     ; 12位(S) : 系统段(0)
;     ; 非系统代码段
;     DESC_S_CODE equ 1_0000_0000_0000b
;     ; 非系统数据段
;     DESC_S_DATA   equ DESC_S_CODE
;     ; 系统段
;     DESC_S_SYS equ 0_0000_0000_0000b
;     ; 8到11位(TYPE) : 描述符的类型
;     ; 只执行代码段
;     DESC_TYPE_CODE equ 1000_0000_0000b
;     ; 可读写数据段,向上扩展
;     ; DESC_TYPE_DATA equ 0100_0000_0000b
;     DESC_TYPE_DATA equ 0010_0000_0000b


;     DESC_CODE_HIGH32  equ (0x00<<24)+DESC_G_4K+DESC_D_32+ \
;     DESC_L_32+DESC_AVL+DESC_LIMIT_CODE_HIGH+ \
;     DESC_P+DESC_DPL_0+DESC_S_CODE+DESC_TYPE_CODE+0x00

;     DESC_DATA_HIGH32  equ (0x00<<24)+DESC_G_4K+DESC_D_32+ \
;     DESC_L_32+DESC_AVL+DESC_LIMIT_DATA_HIGH+DESC_P+DESC_DPL_0+DESC_S_DATA+ \
;     DESC_TYPE_DATA +0X00
;; 159 页有问题