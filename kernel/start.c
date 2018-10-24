#include "kernel.h"
#include "funcs.h"
#include "type.h"
#include "protect.h"

u32 seg2phys(u16 seg)
{
	DESCRIPTOR* p_dest = &gdt[seg >> 3];
	return (p_dest->base_high << 24) | (p_dest->base_mid << 16) | (p_dest->base_low);
}

void initDesc(DESCRIPTOR * p_desc, u32 base, u32 limit, u16 attribute)
{
	p_desc->limit_low		= limit & 0x0FFFF;		// 段界限 1		(2 字节)
	p_desc->base_low		= base & 0x0FFFF;		// 段基址 1		(2 字节)
	p_desc->base_mid		= (base >> 16) & 0x0FF;		// 段基址 2		(1 字节)
	p_desc->attr1			= attribute & 0xFF;		// 属性 1
	p_desc->limit_high_attr2	= ((limit >> 16) & 0x0F) |
						(attribute >> 8) & 0xF0;// 段界限 2 + 属性 2
	p_desc->base_high		= (base >> 24) & 0x0FF;		// 段基址 3		(1 字节)
}

void initIdtDesc(unsigned char vector, u8 desc_type,
			  void (* handler)(), unsigned char privilege)
{
	GATE *	p_gate	= &idt[vector];
	u32	base	= (u32)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= SELECTOR_KERNEL_CS;
	p_gate->dcount		= 0;
	p_gate->attr		= desc_type | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}

void setupTssDes(){
    MemSet(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_DS;
    initDesc(&gdt[INDEX_TSS], seg2phys(SELECTOR_KERNEL_DS)+(u32)&tss, sizeof(TSS)-1, DA_386TSS);
    tss.iobase = sizeof(tss);
}

void setupLDTDes(){
    u16 selectorLdt = INDEX_LDT_FIRST << 3;
    for(int i=0; i<NR_TASKS+NR_USER_PROCS; i++){
        initDesc(&gdt[selectorLdt >> 3], seg2phys(SELECTOR_KERNEL_DS)+(u32)procTable[i].ldts, LDT_SIZE*sizeof(DESCRIPTOR)-1, DA_LDT);
        selectorLdt += (1<<3);
    }
    
}

void initGDTAndSetGDTR(){
    MemCpy(&gdt, (void*)(*((u32*) (&gdtPtr[1]))), *((u16*)(&gdtPtr[0]))+1);
    setupTssDes();
    setupLDTDes();
    u16* pGdtLimit = (u16*)(&gdtPtr[0]);
    u32* pGdtBase = (u32*)(&gdtPtr[1]);
    *pGdtLimit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *pGdtBase = (u32)gdt;
}

void initIdt()
{
    // 全部初始化成中断门(没有陷阱门)
    initIdtDesc(INT_VECTOR_DIVIDE, DA_386IGate, divide_error, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_DEBUG, DA_386IGate, single_step_exception, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_NMI, DA_386IGate, nmi, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_BREAKPOINT, DA_386IGate, breakpoint_exception, PRIVILEGE_USER);
    initIdtDesc(INT_VECTOR_OVERFLOW, DA_386IGate, overflow, PRIVILEGE_USER);
    initIdtDesc(INT_VECTOR_BOUNDS, DA_386IGate, bounds_check, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_INVAL_OP, DA_386IGate, inval_opcode, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_COPROC_NOT, DA_386IGate, copr_not_available, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_COPROC_SEG, DA_386IGate, copr_seg_overrun, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_INVAL_TSS, DA_386IGate, inval_tss, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_SEG_NOT, DA_386IGate, segment_not_present, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_STACK_FAULT, DA_386IGate, stack_exception, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_PROTECTION, DA_386IGate, general_protection, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_PAGE_FAULT, DA_386IGate, page_fault, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_COPROC_ERR, DA_386IGate, copr_error, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);
    initIdtDesc(INT_VECTOR_SYS_CALL, DA_386IGate, SysCall, PRIVILEGE_USER);
}

void init8259A()
{
    //ICW1
    Out(INT_M_CTL, 0x11);
    Out(INT_S_CTL, 0x11);
    //ICW2
    Out(INT_M_CTLMASK, INT_VECTOR_IRQ0);
    Out(INT_S_CTLMASK, INT_VECTOR_IRQ8);
    //ICW3
    Out(INT_M_CTLMASK, 0x4);
    Out(INT_S_CTLMASK, 0x2);
    //ICW4
    Out(INT_M_CTLMASK, 0x1);
    Out(INT_S_CTLMASK, 0x1);
    //OCW1
    Out(INT_M_CTLMASK, 0xFF);
    Out(INT_S_CTLMASK, 0xFF);
}

void initIrqTable(){
    for(int i=0; i<NR_IRQ; i++){
        irqTable[i] = HardwareInt;
    }
}

void initIDTAndSetIDTR(){
    u16* pIdtLimit = (u16*)(&idtPtr[0]);
    u32* pIdtBase = (u32*)(&idtPtr[1]);
    *pIdtLimit = IDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *pIdtBase = (u32)idt;
    initIdt();
    init8259A();
    initIrqTable();

}

void cstart(){
    DispPos=0;
    DispStr("\n------\"CSTART\"-----");
    initGDTAndSetGDTR();
    DispStr("GDT complete!\n");
    initIDTAndSetIDTR();
    DispStr("IDT complete!\n");
    DispStr("------\"CSTART END\"-----\n");
}

/*
1. GDT and GDTR
2. IDT and IDTR
*/