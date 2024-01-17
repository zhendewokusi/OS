%include "boot.inc"
section LOADER vstart=LOADER_BASE_ADDR

    mov byte [es:0x00],'a'
    mov byte [es:0x02],'a'
    mov byte [es:0x04],'a'
    mov byte [es:0x06],'a'
    mov byte [es:0x08],'a'
    mov byte [es:0x0a],'a'
    mov byte [es:0x0c],'a'
    mov byte [es:0x0e],'a'
    mov byte [es:0x10],'a'
    mov byte [es:0x12],'a'
    mov byte [es:0x14],'a'
    mov byte [es:0x16],'a'

    jmp $