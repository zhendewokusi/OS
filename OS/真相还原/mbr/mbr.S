%include "boot.inc"
section MBR vstart=0x7c00   ;这里不能加空格
    ; 选择 80 * 25 文本模式,上卷窗口
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; 设置显示段寄存器
    mov ax,0xb800
    mov es,ax
    ; 设置代码段
    mov ax,0x7c00
    mov ds,ax
    mov bp,0x7c00
    
    mov bx,0
    mov cx,0
    mov dx,0
    ; 输出 Hello,Kernel
    mov byte [es:0x00],'H'
    mov byte [es:0x02],'e'
    mov byte [es:0x04],'l'
    mov byte [es:0x06],'l'
    mov byte [es:0x08],'o'
    mov byte [es:0x0a],','
    mov byte [es:0x0c],'K'
    mov byte [es:0x0e],'e'
    mov byte [es:0x10],'r'
    mov byte [es:0x12],'n'
    mov byte [es:0x14],'e'
    mov byte [es:0x16],'l'

    ; 读取磁盘中的loader到内存中
    mov eax,LOADER_START_SECTOR ; eax 起始的逻辑扇区号
    mov bx,LOADER_BASE_ADDR    ; bx 读取到内存的具体地址
    mov cx,0x01                ; cx 总读取的扇区数
    call rd_disk_m_16
    ; 读取完毕，进入loader
    jmp LOADER_BASE_ADDR

rd_disk_m_16:
    
    ret


    times 510 - ($ - $$) db 0
    db 0x55,0xaa