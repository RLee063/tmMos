%include "sconst.inc"
;macros=========================================================
%macro  hardware_int   1
    call    save

    in	al, INT_M_CTLMASK	; `.
	or	al, (1 << %1)		;  | 屏蔽当前中断
 
    mov     al, EOI
    out     INT_M_CTL, al

	sti
	push	%1
	call	[irqTable + 4 * %1]
	pop     ecx
    cli

    in	al, INT_M_CTLMASK	; \
	and	al, ~(1 << %1)		;  | 恢复接受当前中断
	out	INT_M_CTLMASK, al	; /

    ret

%endmacro

%macro  cpu_int_code 1
    push    %1
    call    CpuInt
    add     esp, 4*2
    hlt
%endmacro

%macro  cpu_int_nocode 1
    push    0xFFFFFFFF
    push    %1
    call    CpuInt
    add     esp, 4*2
    hlt
%endmacro

;extern
extern      cstart
extern      CpuInt
extern      HardwareInt
extern      ClockClick
extern	    KernelMain

extern      irqTable
extern      sysCallTable
extern 	    nextProc
extern      gdtPtr
extern      idtPtr
extern      tss
extern	    DispPos
extern      reEnterFlag
;global
global _start
global restart

global  SysCall
global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15


bits 32
;define
selectorCode     equ     8

[section .bss]
stackSpace      resb    2*1024
stackTop:

[section .text]

_start:
    mov     dword [DispPos], 0
    mov     ah, 95h
    mov     al, 'K'
    mov     [gs:((80*1+39)*2)], ax

    mov     esp, stackTop
    sgdt    [gdtPtr]
    call    cstart
    lgdt    [gdtPtr]
    lidt    [idtPtr]
    jmp     selectorCode:csinit 

csinit:
    mov     ah, 95h
    mov     al, 'X'
    mov     [gs:((80*1+40)*3)], ax

    xor     eax, eax
    mov     ax, SELECTOR_TSS
    ltr     ax

    jmp     KernelMain
divide_error:
    cpu_int_nocode  0
single_step_exception:
    cpu_int_nocode  1
nmi:
    cpu_int_nocode  2
breakpoint_exception:
    cpu_int_nocode  3
overflow:
    cpu_int_nocode  4
bounds_check:
    cpu_int_nocode  5
inval_opcode:
    cpu_int_nocode  6
copr_not_available:
    cpu_int_nocode  7
double_fault:
    cpu_int_code    8
copr_seg_overrun:
    cpu_int_nocode  9
inval_tss:
    cpu_int_code    10
segment_not_present:
    cpu_int_code    11
stack_exception:
    cpu_int_code    12
general_protection:
    cpu_int_nocode  13
page_fault:
    cpu_int_code    14
copr_error:
    cpu_int_nocode  16
hwint00: 
    hardware_int    0
hwint01:
    hardware_int    1
hwint02:
    hardware_int    2
hwint03:
    hardware_int    3
hwint04:
    hardware_int    4
hwint05:
    hardware_int    5
hwint06:
    hardware_int    6
hwint07:
    hardware_int    7
hwint08:
    hardware_int    8
hwint09:
    hardware_int    9
hwint10:
    hardware_int    10
hwint11:
    hardware_int    11
hwint12:
    hardware_int    12
hwint13:
    hardware_int    13
hwint14:
    hardware_int    14
hwint15:
    hardware_int    15


SysCall:
    call    save
    push	dword [nextProc]
    sti
    push	ecx
    push	ebx
    call    [sysCallTable + eax*4]
    add     esp, 4*3
    mov     [esi + EAXREG - P_STACKBASE], eax;save returenvalue
    cli
    ret
;===================================================
;                   SAVE
save:
    pushad
    push    ds
    push    es
    push    fs
    push    gs
    mov     dx, ss
    mov     ds, dx
    mov     es, dx

    mov     esi, esp

    inc     byte [gs:0]

    inc     dword [reEnterFlag]
    cmp     dword [reEnterFlag], 0
    jne     .reenter
    mov	    esp, stackTop		; 切到内核栈
    push    restart
    jmp     [esi + RETADR - P_STACKBASE]
.reenter:
    push    restart_retern
    jmp     [esi + RETADR - P_STACKBASE]


;                   RESTART
restart:
    mov	esp, [nextProc]	; 离开内核栈
	lldt	[esp + P_LDT_SEL]
	lea	esi, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], esi
restart_retern:
    dec     dword [reEnterFlag]
    pop     gs
    pop     fs
    pop     es
    pop     ds
    popad
    add esp, 4
    iretd