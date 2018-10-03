#include "const.h"
#include "funcs.h"
#include "type.h"
#include "protect.h"

int		DispPos;
u16		gdtPtr[3];	/* 0~15:Limit  16~47:Base */
DESCRIPTOR	gdt[GDT_SIZE];
u16		idtPtr[3];	/* 0~15:Limit  16~47:Base */
GATE		idt[IDT_SIZE];

void cstart(){
	DispPos = 0;
    MemCpy(&gdt, (void*)(*((u32*) (&gdtPtr[1]))), *((u16*)(&gdtPtr[0])));
    u16* pGdtLimit = (u16*)(&gdtPtr[0]);
    u32* pGdtBase = (u32*)(&gdtPtr[1]);
    *pGdtLimit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *pGdtBase = (u32)gdt;

    u16* pIdtLimit = (u16*)(&idtPtr[0]);
    u32* pIdtBase = (u32*)(&idtPtr[1]);
    *pIdtLimit = IDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *pIdtBase = (u32)idt;
    DispStr("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n------\"CSTART\"-----");
    DispStr("\n------\"CSTART END\"-----");
    initIdt();
	DispPos = 0;
}
