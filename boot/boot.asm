;%define DEBUG
%ifdef DEBUG
    org     0100h
%else
    org     7c00h
%endif

; BS_jmpBoot
    jmp short bootStart
    nop

%include    "FAT12.inc"

bootStart:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     esp, baseOfStack
    mov     ax, bootString
    mov     cx, bootStringL
    call    showMessage
    mov     ax, baseOfLoader
    mov     es, ax
    mov     bx, offsetOfLoader
    mov     si, loaderName
    mov     cx, loaderNameL
    call    readFile
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderReady
    mov     cx, loaderReadyL
    call    showMessage
    jmp     baseOfLoader:offsetOfLoader

%include    "readSector.inc"
%include    "showMessage.inc"
%include    "readFile.inc"

;string table
bootString          DB  'Hello! This is tmMos OS!'
bootStringL         equ $-bootString
loaderName          DB  'LOADER0 COM'
loaderNameL         equ $-loaderName
loaderReady         DB  'LOADER READY!'
loaderReadyL        equ $-loaderReady

;restAndEndOfBoot
times 510-($-$$)    DB  0
DW  0xaa55               ;end of boot

;#define

%ifdef  DEBUG
baseOfStack     equ     0100h
%else
baseOfStack     equ     07c00h
%endif

baseOfLoader            equ     09000h   
offsetOfLoader          equ     0100h



        