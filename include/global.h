#pragma once
#include "const.h"
#include "type.h"
#include "protect.h"

extern int		DispPos;
extern u8		gdtPtr[6];	/* 0~15:Limit  16~47:Base */
extern DESCRIPTOR	gdt[GDT_SIZE];
extern u8		idtPtr[6];	/* 0~15:Limit  16~47:Base */
extern GATE		idt[IDT_SIZE];
