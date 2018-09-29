%ifdef	SHOW_MESSAGE
%else
%define	SHOW_MESSAGE
;ax = address(string)
;cx = length(string)
showMessage:
    push    bp
    push    bx
    mov     bp, ax          ;ES:BP string's address
    mov     ax, 01301h      ;AH 13h, AL 01h
    mov     bx, 000ch       ;BH: numOfPage, BL=0ch: highlight
    mov     dl, 0
    int     10h
    pop     bx
    pop     bp
    ret
%endif