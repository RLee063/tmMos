;这个文件不好


baseOfLoader		equ	 09000h	; LOADER.BIN 被加载到的位置 ----  段地址
offsetOfLoader		equ	  0100h	; LOADER.BIN 被加载到的位置 ---- 偏移地址

baseOfLoaderPhyAddr	equ	baseOfLoader * 10h	; LOADER.BIN 被加载到的位置 ---- 物理地址 (= BaseOfLoader * 10h)

baseOfKernel	equ	 08000h	; KERNEL.BIN 被加载到的位置 ----  段地址
offsetOfKernel	equ	     0h	; KERNEL.BIN 被加载到的位置 ---- 偏移地址

baseOfKernelPhyAddr	equ	baseOfKernel * 10h
kernelEntryPointPhyAddr	equ	030400h	; 注意：1、必须与 MAKEFILE 中参数 -Ttext 的值相等!!
					;       2、这是个地址而非仅仅是个偏移，如果 -Ttext 的值为 0x400400，则它的值也应该是 0x400400。

pageDirBase		equ	200000h	; 页目录开始地址:		2M
pageTblBase		equ	201000h	; 页表开始地址:			2M + 4K

