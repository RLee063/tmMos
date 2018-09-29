org 0h
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

    jmp     $

;functionss
%include "readFile.inc"
%include "showMessage.inc"

;stringTable
welcomeString   DB  'Now in kernel.bin'
welcomeStringL  equ $-welcomeString



;define
baseOfStack     equ     0h