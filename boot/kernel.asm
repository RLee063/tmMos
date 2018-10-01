[section .text]

global _start

_start:
    mov     ah, 95h
    mov     al, 'K'
    mov     [gs:((80*1+39)*2)], ax
    jmp     $