selctorCode     equ     8

extern      cstart
extern      gdtPtr

[section .bss]
stackSpace      resb    2*1024
stackTop

[section .text]
global _start
_start:
    mov     ah, 95h
    mov     al, 'K'
    mov     [gs:((80*1+39)*2)], ax

    mov     esp, stackTop
    sgdt    [gdtPtr]
    call    cstart
    lgdt    [gdtPtr]
    jmp     selctorCode:csinit 

csinit:
    push    0
    popfd

    mov     ah, 95h
    mov     al, 'X'
    mov     [gs:((80*1+40)*3)], ax
    jmp     $

    hlt

