#pragma once
#include "type.h"
#include "protect.h"
#include "kernel.h"
extern int		DispPos;
extern u16		gdtPtr[3];	/* 0~15:Limit  16~47:Base */
extern DESCRIPTOR	gdt[];
extern u16		idtPtr[3];	/* 0~15:Limit  16~47:Base */
extern GATE	    idt[];

extern TSS      tss;
extern PROCESS  procTable[];
extern TASK     taskTable[]; 
extern char     taskStack[];

extern int      reEnterFlag;
extern PROCESS* nextProc;
