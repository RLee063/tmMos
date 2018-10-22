%include "protectMode.inc"
%include "FAT12.inc"
%include "loadConvention.inc"
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

    mov     ax, ds
    mov     es, ax
    mov     ax, kernelReady
    mov     cx, kernelReadyL
    call    showMessage

    ; 得到内存数
	mov	ebx, 0			; ebx = 后续值, 开始时需为 0
	mov	di, _MemChkBuf		; es:di 指向一个地址范围描述符结构(ARDS)
.MemChkLoop:
	mov	eax, 0E820h		; eax = 0000E820h
	mov	ecx, 20			; ecx = 地址范围描述符结构的大小
	mov	edx, 0534D4150h		; edx = 'SMAP'
	int	15h			; int 15h
	jc	.MemChkFail
	add	di, 20
	inc	dword [_dwMCRNumber]	; dwMCRNumber = ARDS 的个数
	cmp	ebx, 0
	jne	.MemChkLoop
	jmp	readyForPM
.MemChkFail:
	mov	dword [_dwMCRNumber], 0

readyForPM:
;ready for protected mode
    call    killMotor
    ;1.gdt
    lgdt    [gdtPtr]   
    ;2.close int 
    cli       
    ;3.open A20         
    in	al, 92h         
	or	al, 00000010b
	out	92h, al   
    ;4.cr0                 
    mov	eax, cr0        
	or	eax, 1
	mov	cr0, eax        
    ;5.jmp to protected mode
    jmp dword selectorCode:(baseofLoaderPhyAddr+protectStart)


;functionss
%include "readFile.inc"
%include "showMessage.inc"

killMotor:
	push	dx
	mov	dx, 03F2h
	mov	al, 0
	out	dx, al
	pop	dx
	ret
;stringTable
welcomeString   DB  'Now in loader.bin'
welcomeStringL  equ $-welcomeString
kernelName      DB  'KERNEL  BIN'
kernelNameL     equ $-kernelName
kernelReady     DB  'kernel has Ready!'
kernelReadyL    equ $-kernelReady

;GDT (Descriptor in "protectMode.inc")
;LABEL                  BASE,   LIMIT,      ATTRIBUTE
GDT_NULL:   Descriptor  0,      0,          0
GDT_CODE:   Descriptor  0,      0fffffh,    DA_CR|DA_32|DA_LIMIT_4K
GDT_DATA:   Descriptor  0,      0fffffh,    DA_DRW|DA_32|DA_LIMIT_4K
GDT_VIDEO:  Descriptor  0B8000h,0fffffh,    DA_DRW|DA_DPL3

gdtLen      equ         $-GDT_NULL
gdtPtr      DW	        gdtLen-1
            DD          baseofLoaderPhyAddr + GDT_NULL

selectorCode    equ     GDT_CODE - GDT_NULL
selectorData    equ     GDT_DATA - GDT_NULL
selectorVideo   equ     GDT_VIDEO - GDT_NULL + SA_RPL3

;define
baseOfStack     equ     0100h

;loaderInfo
baseofLoaderPhyAddr     equ     090000h
;==============================================32 bit==========
[section .code32]
align 32
[bits 32]

protectStart:
    mov     ax, selectorVideo
    mov     gs, ax
    mov	    ax, selectorData
	mov	    ds, ax
	mov	    es, ax
	mov	    fs, ax
	mov	    ss, ax
	mov	    esp, TopOfStack

    push	szMemChkTitle
	call	DispStr
	add	    esp, 4
	call	dispMemInfo
	call	SetupPaging

	mov	ah, 95h				; 0000: 黑底    1111: 白字
	mov	al, 'P'
	mov	[gs:((80 * 0 + 39) * 2)], ax	; 屏幕第 0 行, 第 39 列
	
    call    initKernel
    
    jmp     selectorCode:kernelEntryPointPhyAddr


;functions
%include "lib.inc"

initKernel:
        xor   esi, esi
        mov   cx, word [baseOfKernelPhyAddr+2Ch];`. ecx <- pELFHdr->e_phnum
        movzx ecx, cx                               ;/
        mov   esi, [baseOfKernelPhyAddr + 1Ch]  ; esi <- pELFHdr->e_phoff
        add   esi, baseOfKernelPhyAddr;esi<-OffsetOfKernel+pELFHdr->e_phoff
.begin:
        mov   eax, [esi + 0]
        cmp   eax, 0                      ; PT_NULL
        jz    .noAction
        push  dword [esi + 010h]    ;size ;`.
        mov   eax, [esi + 04h]            ; |
        add   eax, baseOfKernelPhyAddr; | memcpy((void*)(pPHdr->p_vaddr),
        push  eax		    ;src  ; |      uchCode + pPHdr->p_offset,
        push  dword [esi + 08h]     ;dst  ; |      pPHdr->p_filesz;
        call  MemCpy                      ; |
        add   esp, 12                     ;/
.noAction:
        add   esi, 020h                   ; esi += pELFHdr->e_phentsize
        dec   ecx
        jnz   .begin
        ret

SetupPaging:
	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
	div	ebx
	mov	ecx, eax	; 此时 ecx 为页表的个数，也即 PDE 应该的个数
	test	edx, edx
	jz	.no_remainder
	inc	ecx		; 如果余数不为 0 就需增加一个页表
.no_remainder:
	push	ecx		; 暂存页表个数

	; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.

	; 首先初始化页目录
	mov	ecx, 1024
	mov	ax, selectorData
	mov	es, ax
	mov	edi, pageDirBase	; 此段首地址为 PageDirBase
	xor	eax, eax
	mov	eax, pageTblBase | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.1

	; 再初始化所有页表
	pop	eax			; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, pageTblBase	; 此段首地址为 PageTblBase
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.2

	mov	eax, pageDirBase
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	ret

; 显示内存信息
dispMemInfo:
	push	esi
	push	edi
	push	ecx

	mov	esi, MemChkBuf
	mov	ecx, [dwMCRNumber];for(int i=0;i<[MCRNumber];i++)//每次得到一个ARDS
.loop:				  ;{
	mov	edx, 5		  ;  for(int j=0;j<5;j++)//每次得到一个ARDS中的成员
	mov	edi, ARDStruct	  ;  {//依次显示:BaseAddrLow,BaseAddrHigh,LengthLow
.1:				  ;               LengthHigh,Type
	push	dword [esi]	  ;
	call	DispInt		  ;    DispInt(MemChkBuf[j*4]); // 显示一个成员
	pop	eax		  ;
	stosd			  ;    ARDStruct[j*4] = MemChkBuf[j*4];
	add	esi, 4		  ;
	dec	edx		  ;
	cmp	edx, 0		  ;
	jnz	.1		  ;  }
	call	DispReturn	  ;  printf("\n");
	cmp	dword [dwType], 1 ;  if(Type == AddressRangeMemory)
	jne	.2		  ;  {
	mov	eax, [dwBaseAddrLow];
	add	eax, [dwLengthLow];
	cmp	eax, [dwMemSize]  ;    if(BaseAddrLow + LengthLow > MemSize)
	jb	.2		  ;
	mov	[dwMemSize], eax  ;    MemSize = BaseAddrLow + LengthLow;
.2:				  ;  }
	loop	.loop		  ;}
				  ;
	call	DispReturn	  ;printf("\n");
	push	szRAMSize	  ;
	call	DispStr		  ;printf("RAM size:");
	add	esp, 4		  ;
				  ;
	push	dword [dwMemSize] ;
	call	DispInt		  ;DispInt(MemSize);
	add	esp, 4		  ;

	pop	ecx
	pop	edi
	pop	esi
	ret

[SECTION .data1]
ALIGN	32
LABEL_DATA:
; 实模式下使用这些符号
; 字符串
_szMemChkTitle:	db "BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0
_szRAMSize:	db "RAM size:", 0
_szReturn:	db 0Ah, 0
;; 变量
_dwMCRNumber:	dd 0	; Memory Check Result
_dwDispPos:	dd (80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列
_dwMemSize:	dd 0
_ARDStruct:	; Address Range Descriptor Structure
  _dwBaseAddrLow:		dd	0
  _dwBaseAddrHigh:		dd	0
  _dwLengthLow:			dd	0
  _dwLengthHigh:		dd	0
  _dwType:			dd	0
_MemChkBuf:	times	256	db	0
;
;; 保护模式下使用这些符号
szMemChkTitle		equ	baseofLoaderPhyAddr + _szMemChkTitle
szRAMSize		equ	baseofLoaderPhyAddr + _szRAMSize
szReturn		equ	baseofLoaderPhyAddr + _szReturn
dwDispPos		equ	baseofLoaderPhyAddr + _dwDispPos
dwMemSize		equ	baseofLoaderPhyAddr + _dwMemSize
dwMCRNumber		equ	baseofLoaderPhyAddr + _dwMCRNumber
ARDStruct		equ	baseofLoaderPhyAddr + _ARDStruct
	dwBaseAddrLow	equ	baseofLoaderPhyAddr + _dwBaseAddrLow
	dwBaseAddrHigh	equ	baseofLoaderPhyAddr + _dwBaseAddrHigh
	dwLengthLow	equ	baseofLoaderPhyAddr + _dwLengthLow
	dwLengthHigh	equ	baseofLoaderPhyAddr + _dwLengthHigh
	dwType		equ	baseofLoaderPhyAddr + _dwType
MemChkBuf		equ	baseofLoaderPhyAddr + _MemChkBuf


;hypoStack
StackSpace: 	times 1024	db	    0
TopOfStack  	equ 	baseofLoaderPhyAddr + $	
 
