
; ------------------------------------------------------------------------
; 显示 AL 中的数字
; ------------------------------------------------------------------------
extern		DispPos

[section .text]
global		DispStr
global		MemCpy
global		DispColorStr
global		Out
global		In

; ------------------------------------------------------------------------
; 显示一个字符串
; ------------------------------------------------------------------------
DispStr:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [DispPos]
	mov	ah, 0Fh
.11:
	lodsb
	test	al, al
	jz	.22
	cmp	al, 0Ah	; 是回车吗?
	jnz	.33
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.11
.33:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.11

.22:
	mov	[DispPos], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
; DispStr 结束------------------------------------------------------------

; ------------------------------------------------------------------------
; 内存拷贝，仿 memcpy
; ------------------------------------------------------------------------
; void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
; ------------------------------------------------------------------------
MemCpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov	al, [ds:esi]		; ┓
	inc	esi			; ┃
					; ┣ 逐字节移动
	mov	byte [es:edi], al	; ┃
	inc	edi			; ┛

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
; MemCpy 结束-------------------------------------------------------------

;void out (u16 port, u8 value)

Out:
	mov		edx, [esp + 4]
	mov		al,	[esp + 8]
	out     	dx, al
	nop
	nop
	ret

;u8	in (u16 port)

In:
	mov		edx, [esp + 4]
	xor		eax, eax
	in		al, dx
	nop
	nop
	ret

;

; ========================================================================
;                  void disp_color_str(char * info, int color);
; ========================================================================
DispColorStr:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pszInfo
	mov	edi, [DispPos]
	mov	ah, [ebp + 12]	; color
.1:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[DispPos], edi

	pop	ebp
	ret

;================================================================

