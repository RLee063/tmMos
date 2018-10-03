
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            const.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#pragma once
#ifndef	_ORANGES_CONST_H_
#define	_ORANGES_CONST_H_


/* 函数类型 */
#define	PUBLIC		/* PUBLIC is the opposite of PRIVATE */
#define	PRIVATE	static	/* PRIVATE x limits the scope of x */

/* GDT 和 IDT 中描述符的个数 */
#define	GDT_SIZE	128
#define IDT_SIZE    256

#endif /* _ORANGES_CONST_H_ */

/*          8259A           */
#define INT_M_CTL       0x20
#define INT_M_CTLMASK   0x21
#define INT_S_CTL       0xA0
#define INT_S_CTLMASK   0xA1

#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

/* 权限 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3