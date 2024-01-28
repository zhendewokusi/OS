%include "boot.inc"
section MBR vstart=0x7c00   ;这里不能加空格
    ; 选择 80 * 25 文本模式,上卷窗口
    mov ah, 0x00
    mov al, 0x03
    int 0x10

    ; 设置显示段寄存器
    mov ax,cs
    mov ds,ax
    mov ax,0xb800
    mov es,ax
    mov sp,0x7c00
    
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
    mov byte [es:0x0c],'M'
    mov byte [es:0x0e],'B'
    mov byte [es:0x10],'R'
    
    ; 由于loader的磁盘以及内存地址都是固定的,默认将其放置在MBR下一个扇区以及后面
    ; 所以不需要传入使用哪个通道,以及主从盘之类的数据
    ; 读取磁盘中的loader到内存中
    mov eax,LOADER_START_SECTOR ; eax 起始的逻辑扇区号
    mov bx,LOADER_BASE_ADDR    ; bx 读取到内存的具体地址
    mov cx,0x02                ; cx 总读取的扇区数
    xchg bx,bx

    call rd_disk_16
    ; 读取完毕，进入loader
    jmp LOADER_BASE_ADDR

; eax 起始的逻辑扇区号
; bx 读取到内存的具体地址
; cx 总读取的扇区数
rd_disk_16:
    mov esi,eax
    mov di,cx
    mov ax,cx


    mov dx,0x1f2
    out dx,al
    mov cl,8
    mov eax,esi

    mov dx,0x1f3
    out dx,al

    shr eax,cl
    mov dx,0x1f4
    out dx,al

    shr eax,cl
    mov dx,0x1f5
    out dx,al

    shr eax,cl
    and al,0x0f
    or al,0xe0
    mov dx,0x1f6
    out dx,al
    
    ; 读命令
    mov al,0x20
    mov dx,0x1f7
    out dx,al
.readying:
    nop
    in al, dx
    test al, 0x80 ; 检查状态寄存器的 bsy 位
    jnz .readying ; 如果 bsy 位为 1，等待磁盘准备好
    ; 获取需要读取多少次,一次一个字所以是512 / 2
    mov ax,di
    mov dx,256
    mul dx
    mov ecx,0
    mov cx,ax
    mov dx,0x1f0
.for_memory:
    in ax,dx
    mov [bx],ax
    add bx,2
    loop .for_memory

.error:
    ; 错误处理
    ; 根据错误类型进行处理
    ret

    times 510 - ($ - $$) db 0
    db 0x55,0xaa