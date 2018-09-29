org 0100h
    mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, baseOfStack

;    mov     ax, 0B800h
;    mov     gs, ax
;    mov     ah, 0Fh
;    mov     al, 'L'
;    mov     [gs:((80*0 + 39) * 2)], ax

    mov     ax, welcomeString
    mov     cx, welcomeStringL
    call    showMessage

    mov     ax, baseOfKernel
    mov     es, ax
    mov     bx, offsetOfKernel
    mov     si, kernelName
    mov     cx, kernelNameL
    call    readFile

    mov     ax, kernelReady
    mov     cx, kernelReadyL
    call    showMessage

    jmp     baseOfKernel:offsetOfKernel

;functionss
%include "readFile.inc"
%include "showMessage.inc"

;stringTable
welcomeString   DB  'Now in loader.bin'
welcomeStringL  equ $-welcomeString
kernelName      DB  'KERNEL  BIN'
kernelNameL     equ $-kernelName
kernelReady     DB  'kernel has Ready!'
kernelReadyL    equ $-kernelReady



;define
baseOfStack     equ     0100h
baseOfKernel    equ     08000h
offsetOfKernel  equ     0h