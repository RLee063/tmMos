#include "type.h"
#include "const.h"
#include "protect.h"

void MemCpy(void* pDst, void* pSrc, u32 size);
void DispStr(void* pStr);

u16  gdtPtr[3];
DESCRIPTOR gdt[GDT_SIZE];

void cstart(){
    MemCpy(&gdt, (void*)(*((u32*) (&gdtPtr[1]))), *((u16*)(&gdtPtr[0])));
    u16* pGdtLimit = (u16*)(&gdtPtr[0]);
    u32* pGdtBase = (u32*)(&gdtPtr[1]);
    *pGdtLimit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *pGdtBase = (u32)gdt;
    DispStr("\n\n\n\n\n\n\n\n\n------\"CSTART\"-----");
	
    DispStr("\n\n\n\n\n\n\n\n\n\n------\"CSTART END\"-----");
}
