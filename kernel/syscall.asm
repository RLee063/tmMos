;=====================================================
;					SYSCALL	
;=====================================================

global  GetTicks
global  Write

%include "sconst.inc"

GetTicks:
	mov	eax, NR_GetTicks
	int		INT_VECTOR_SYS_CALL
	ret

Write:
    mov eax, NR_Write
    mov     ebx, [esp + 4]
    mov     ecx, [esp + 8]
    int     INT_VECTOR_SYS_CALL
    ret