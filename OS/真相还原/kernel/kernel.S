[bits 32]
%define ERROR_CODE nop
%define ZERO push 0

extern idt_table		 ;idt_table是C中注册的中断处理程序数组

%macro VECTOR 2

section .text
intr%1entry:
    ; 压入ERROR_CODE 或者 ZERO（占位）
    %2
    ; 保存上下文
    push ds
    push es
    push fs
    push gs
    pushad
    ; 发送 EOI
    mov al,0x20
    out 0x20,al
    out 0xa0,al
    ; 保存中断向量号
    push %1
    ; 调用 C 版本的中断处理函数
    call [idt_table + %1*4]       ; 调用idt_table中的C版本中断处理函数
.exit:
    add esp,4   ;跳过中断号
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp,4   ;跳过ERROR_CODE或者ZERO
    iret

section .data
    dd intr%1entry
%endmacro

section .data
global intr_entry_table
intr_entry_table:
    VECTOR 0x00,ZERO
    VECTOR 0x01,ZERO
    VECTOR 0x02,ZERO
    VECTOR 0x03,ZERO 
    VECTOR 0x04,ZERO
    VECTOR 0x05,ZERO
    VECTOR 0x06,ZERO
    VECTOR 0x07,ZERO 
    VECTOR 0x08,ERROR_CODE
    VECTOR 0x09,ZERO
    VECTOR 0x0a,ERROR_CODE
    VECTOR 0x0b,ERROR_CODE 
    VECTOR 0x0c,ZERO
    VECTOR 0x0d,ERROR_CODE
    VECTOR 0x0e,ERROR_CODE
    VECTOR 0x0f,ZERO 
    VECTOR 0x10,ZERO
    VECTOR 0x11,ERROR_CODE
    VECTOR 0x12,ZERO
    VECTOR 0x13,ZERO 
    VECTOR 0x14,ZERO
    VECTOR 0x15,ZERO
    VECTOR 0x16,ZERO
    VECTOR 0x17,ZERO 
    VECTOR 0x18,ERROR_CODE
    VECTOR 0x19,ZERO
    VECTOR 0x1a,ERROR_CODE
    VECTOR 0x1b,ERROR_CODE 
    VECTOR 0x1c,ZERO
    VECTOR 0x1d,ERROR_CODE
    VECTOR 0x1e,ERROR_CODE
    VECTOR 0x1f,ZERO 
    VECTOR 0x20,ZERO
