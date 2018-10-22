;=====================================================
;					SYSCALL	
;=====================================================

global  GetTicks
global  Write
global  SendRecv

%include "sconst.inc"

GetTicks:
	mov	eax, NR_GetTicks
	int		INT_VECTOR_SYS_CALL
	ret

SendRecv:
    mov eax, NR_SendRecv
    mov     ebx, [esp + 4]  ;function
    mov     ecx, [esp + 8]  ;serDest
    mov     edx, [esp + 12] ;pMsg
    int     INT_VECTOR_SYS_CALL
    ret

Write:
    mov eax, NR_Write
    mov     ecx, [esp + 4];buf
    mov     edx, [esp + 8];length
    int     INT_VECTOR_SYS_CALL
    ret