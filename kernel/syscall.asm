;=====================================================
;					SYSCALL	
;=====================================================

global  printx
global  SendRecv

%include "sconst.inc"

; GetTicks:
; 	mov	eax, NR_GetTicks
; 	int		INT_VECTOR_SYS_CALL
; 	ret

SendRecv:
    mov eax, NR_SendRecv
    mov     ebx, [esp + 4]  ;function
    mov     ecx, [esp + 8]  ;serDest
    mov     edx, [esp + 12] ;pMsg
    int     INT_VECTOR_SYS_CALL
    ret

printx:
    mov eax, NR_printx
    mov     ecx, [esp + 4];buf
    int     INT_VECTOR_SYS_CALL
    ret