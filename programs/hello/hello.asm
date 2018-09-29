helloString:    db  "Hello, the world!"
helloStringL:   equ $-helloString

global  _start

_start:
    mov     edx, helloStringL
    mov     ecx, helloString
    mov     ebx, 1
    mov     eax, 4
    int     0x80
    mov     ebx, 0
    mov     eax, 1
    int     0x80
