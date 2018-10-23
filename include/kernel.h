#pragma once
#include "type.h"
#include "protect.h"
/* 其他 */
#define TRUE 	1
#define FALSE 	0

/* 函数类型 */
#define	PUBLIC		/* PUBLIC is the opposite of PRIVATE */
#define	PRIVATE	static	/* PRIVATE x limits the scope of x */

/* 权限 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3
//==================


//============================================
//                  GDT/IDT/LDT
//============================================
/* GDT 和 IDT 中描述符的个数 */
#define	GDT_SIZE	128
#define IDT_SIZE    256


/* GDT描述符索引 */
#define	INDEX_DUMMY		0	// ┓
#define	INDEX_FLAT_C		1	// ┣ LOADER 里面已经确定了的.
#define	INDEX_FLAT_RW		2	// ┃
#define	INDEX_VIDEO		3	// ┛
#define	INDEX_TSS		4
#define	INDEX_LDT_FIRST		5
/* GDT选择子 */
#define	SELECTOR_DUMMY		   0		// ┓
#define	SELECTOR_FLAT_C		0x08		// ┣ LOADER 里面已经确定了的.
#define	SELECTOR_FLAT_RW	0x10		// ┃
#define	SELECTOR_VIDEO		(0x18+3)	// ┛<-- RPL=3
#define	SELECTOR_TSS		0x20		// TSS. 从外层跳到内存时 SS 和 ESP 的值从里面获得.
#define SELECTOR_LDT_FIRST	0x28
/* KERNEL GDT选择子 */
#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO

/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3

//LDT
#define LDT_SIZE    2
#define INDEX_LDT_C             0
#define INDEX_LDT_RW            1
//=============================================
//                  进程
//=============================================
typedef struct s_stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;		/* ┓						│			*/
	u32	fs;		/* ┃						│			*/
	u32	es;		/* ┃						│			*/
	u32	ds;		/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()				│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;		/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;

typedef struct s_proc {
	struct s_stackframe			regs;			/* process' registers saved in stack frame */

	u16				ldt_sel;		/* selector in gdt giving ldt base and limit*/
	struct s_descriptor			ldts[LDT_SIZE];		/* local descriptors for code and data */
								/* 2 is LDT_SIZE - avoid include protect.h */
	u32				pid;			/* process id passed in from MM */
	char				p_name[16];		/* name of the process */
	int 			nr_tty;
	PROC_STATUS		status;
	struct s_proc *		senderQueue;
	int				receiveFrom;
	int				sendTo;
	MESSAGE *		msg;
}PROCESS;

//TASK
typedef struct s_task {
	void (*initial_eip)();
	int	stacksize;
	char	name[32];
}TASK;

/* task pid */
#define INVALID_DRIVER	-20
#define INTERRUPT	-10
#define TASK_TTY	0
#define TASK_SYS	1

/* Number of tasks */
#define NR_TASKS	2
#define NR_USER_PROCS	3
#define NR_PROCS	(NR_USER_PROCS + NR_TASKS)

/* stacks of tasks */
#define STACK_SIZE_TESTA		0x8000
#define STACK_SIZE_TESTB		0x8000
#define STACK_SIZE_TESTC		0x8000
#define STACK_SIZE_TTY			0x8000
#define STACK_SIZE_SYSCALL		0x8000

#define STACK_SIZE_TOTAL	NR_PROCS * 0x8000
//=====================================
//                  中断
//=====================================
/* 中断向量 */
#define	INT_VECTOR_IRQ0			0x20
#define	INT_VECTOR_IRQ8			0x28
#define INT_VECTOR_SYS_CALL	0x90

#define NR_IRQ      16
#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ	5	/* xt winchester */
#define	FLOPPY_IRQ	6	/* floppy disk */
#define	PRINTER_IRQ	7
#define	AT_WINI_IRQ	14	/* at winchester */

//=====================================
//                  syscall
//=====================================
#define NR_SYS_CALL				3

#define NR_GetTicks             0
#define NR_SendRecv				1
#define	NR_printx				2

//====================================
//					clock
//====================================
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
			     * Counter0 - LSB then MSB - rate generator - binary
			     */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             100  /* clock freq (software settable on IBM-PC) */

//====================================
//					TTY
//====================================
//consle
#define SCR_UP	1	/* scroll forward */
#define SCR_DN	-1	/* scroll backward */
#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80

#define	KB_IN_BYTES	32	/* size of keyboard input buffer */
#define MAP_COLS	3	/* Number of columns in keymap */
#define NR_SCAN_CODES	0x80	/* Number of scan codes (rows in keymap) */
typedef struct s_kb {
	char*	pHead;			/* 指向缓冲区中下一个空闲位置 */
	char*	pTail;			/* 指向键盘任务应处理的字节 */
	int	count;			/* 缓冲区中共有多少字节 */
	char	buf[KB_IN_BYTES];	/* 缓冲区 */
}KEYBOARD_BUFFER;

#define NR_CONSOLES	3
typedef struct s_console
{
	unsigned int	current_start_addr;	/* 当前显示到了什么位置	  */
	unsigned int	original_addr;		/* 当前控制台对应显存位置 */
	unsigned int	v_mem_limit;		/* 当前控制台占的显存大小 */
	unsigned int	cursor;			/* 当前光标位置 */
}CONSOLE;
//tty
#define TTY_IN_BYTES	256	/* tty input queue size */
typedef struct s_tty
{
	u32	in_buf[TTY_IN_BYTES];	/* TTY 输入缓冲区 */
	u32*	p_inbuf_head;		/* 指向缓冲区中下一个空闲位置 */
	u32*	p_inbuf_tail;		/* 指向键盘任务应处理的键值 */
	int	inbuf_count;		/* 缓冲区中已经填充了多少 */

	struct s_console *	p_console;
}TTY;

//=====================================
//					键盘
//=====================================
#define FLAG_BREAK	0x0080		/* Break Code			*/
#define FLAG_EXT	0x0100		/* Normal function keys		*/
#define FLAG_SHIFT_L	0x0200		/* Shift key			*/
#define FLAG_SHIFT_R	0x0400		/* Shift key			*/
#define FLAG_CTRL_L	0x0800		/* Control key			*/
#define FLAG_CTRL_R	0x1000		/* Control key			*/
#define FLAG_ALT_L	0x2000		/* Alternate key		*/
#define FLAG_ALT_R	0x4000		/* Alternate key		*/
#define FLAG_PAD	0x8000		/* keys in num pad		*/

//=====================================
//				SEND_RECV
//=====================================
//Message type
#define SEND		1
#define RECEIVE		2
#define BOTH		3	/* BOTH = (SEND | RECEIVE) */

//who
#define ANY		(NR_PROCS + 10)
#define NO_TASK		(NR_PROCS + 20)



struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void*	m3p1;
	void*	m3p2;
};
struct _msg{
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
};

#define	RETVAL		u.m3.m3i1

//========================================
//				print
//========================================
// color
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */
#define	MAKE_COLOR(x,y)	((x<<4) | y) /* MAKE_COLOR(Background,Foreground) */


/* magic chars used by `printx' */
#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'
#define DEFAULT_CHAR_COLOR	0x07	/* 0000 0111 黑底白字 */
#define GRAY_CHAR		(MAKE_COLOR(BLACK, BLACK) | BRIGHT)
#define RED_CHAR		(MAKE_COLOR(BLUE, RED) | BRIGHT)

#define STR_DEFAULT_LEN 1024