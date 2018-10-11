#pragma once
#include "type.h"
#include "protect.h"
#include "kernel.h"
#ifdef  _GLOBAL_
#define EXTERN 
#else
#define EXTERN extern
#endif

//LIB
EXTERN int		DispPos;
EXTERN int      ticks;
//GDT
EXTERN u16		gdtPtr[3];	/* 0~15:Limit  16~47:Base */
EXTERN DESCRIPTOR	gdt[GDT_SIZE];
//IDT
EXTERN u16		idtPtr[3];	/* 0~15:Limit  16~47:Base */
EXTERN GATE	    idt[IDT_SIZE];
//TSS
EXTERN TSS      tss;
//PROCESS
EXTERN PROCESS  procTable[NR_TASKS];
EXTERN int      reEnterFlag;
EXTERN PROCESS* nextProc;
//TASK
EXTERN TASK     taskTable[NR_TASKS]; 
EXTERN char     taskStack[STACK_SIZE_TOTAL];
//IRQ
EXTERN void*   irqTable[NR_IRQ];
//SYSCALL
EXTERN void*   sysCallTable[NR_SYS_CALL];
//TTY
EXTERN KEYBOARD_BUFFER keyboardInput;
EXTERN u32      keymap[NR_SCAN_CODES * MAP_COLS];

EXTERN TTY      ttyTable[NR_CONSOLES];
EXTERN CONSOLE  consoleTable[NR_CONSOLES];
EXTERN int      nrCurrentConsole;