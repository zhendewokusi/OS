%include "boot.inc"
section LOADER vstart=0x8000
    mov ax,0xb800
    mov es,ax

    mov byte [es:0x00],'a'
    mov byte [es:0x02],'b'
    mov byte [es:0x04],'c'
    mov byte [es:0x06],'d'
    mov byte [es:0x08],'e'
    mov byte [es:0x0a],'f'
    mov byte [es:0x0c],'g'
    mov byte [es:0x0e],'a'
    mov byte [es:0x10],'a'
    mov byte [es:0x12],'a'
    mov byte [es:0x14],'a'
    mov byte [es:0x16],'a'

    jmp $