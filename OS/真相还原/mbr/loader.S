%include "boot.inc"
; 目标:
; 开A20地址线
; 加载gdt表
; 将cr0的PE位置为1

section LOADER vstart=LOADER_BASE_ADDR
    jmp loader_start
 GDT_BASE:
     dd 0x0000_0000
     dd 0x0000_0000
 CODE_DESC:
     dd 0x0000FFFF
     dd DESC_CODE_HIGH32
 STACK_DESC:
     dd 0x0000FFFF
     dd DESC_DATA_HIGH32
 
 GDT_LIMIT equ ($ - GDT_BASE) - 1
     times 60 dq 0   ;预留描述符的空位
 
 SECTOR_CODE  equ  (0x01 << 3) + TI_GDT +  RPL0
 SECTOR_STACK equ  (0x02 << 3) + TI_GDT +  RPL0
 
 gdt_ptr dw GDT_LIMIT
         dd GDT_BASE
loader_start:
    mov ax,0xb800
    mov es,ax
    mov di,80*2 ;这里是 80*25 显示模式,且 2 字节输出一个字符,有一个是属性,这里是为了换行
    mov byte [es:di+0x00],'H'
    mov byte [es:di+0x02],'e'
    mov byte [es:di+0x04],'l'
    mov byte [es:di+0x06],'l'
    mov byte [es:di+0x08],'o'
    mov byte [es:di+0x0a],','
    mov byte [es:di+0x0c],'L'
    mov byte [es:di+0x0e],'O'
    mov byte [es:di+0x10],'A'
    mov byte [es:di+0x12],'D'
    mov byte [es:di+0x14],'E'
    mov byte [es:di+0x16],'R'
    
    ; 打开A20地址线
    in al,0x92
    or al, 0000_0010B
    out 0x92,al

    ; 加载 GDT 表
    lgdt [gdt_ptr]
    ; 将cr0的PE位置为1
    mov eax,cr0
    or eax,0x00000001
    mov cr0,eax
    ; 刷新流水线
    jmp dword SECTOR_CODE:protect_start
    [bits 32]
protect_start:

    jmp $

