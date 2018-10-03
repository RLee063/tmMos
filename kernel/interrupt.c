#include "type.h"
#include "funcs.h"
#include "const.h"
#include "protect.h"
#include "global.h"

void init8259A();

void initIdtDesc(unsigned char vector, u8 desc_type, void (*handler)(), unsigned char privilege);

void initIdt()
{
    init8259A();
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
    Out(INT_M_CTLMASK, 0xFD);
    Out(INT_S_CTLMASK, 0xFF);
}

void CpuInt(int vecNo, int errCode, int eip, int cs, int eflags)
{
    int i;
	int text_color = 0x74; /* 灰底红字 */

	char * err_msg[] = {"#DE Divide Error",
			    "#DB RESERVED",
			    "—  NMI Interrupt",
			    "#BP Breakpoint",
			    "#OF Overflow",
			    "#BR BOUND Range Exceeded",
			    "#UD Invalid Opcode (Undefined Opcode)",
			    "#NM Device Not Available (No Math Coprocessor)",
			    "#DF Double Fault",
			    "    Coprocessor Segment Overrun (reserved)",
			    "#TS Invalid TSS",
			    "#NP Segment Not Present",
			    "#SS Stack-Segment Fault",
			    "#GP General Protection",
			    "#PF Page Fault",
			    "—  (Intel reserved. Do not use.)",
			    "#MF x87 FPU Floating-Point Error (Math Fault)",
			    "#AC Alignment Check",
			    "#MC Machine Check",
			    "#XF SIMD Floating-Point Exception"
	};

	/* 通过打印空格的方式清空屏幕的前五行，并把 DispPos 清零 */
	DispPos = 0;
	for(i=0;i<80*5;i++){
		DispStr(" ");
	}
	DispPos = 0;

	DispColorStr("Exception! --> ", text_color);
	DispColorStr(err_msg[vecNo], text_color);
	DispColorStr("\n\n", text_color);
	DispColorStr("EFLAGS:", text_color);
	DispInt(eflags);
	DispColorStr("CS:", text_color);
	DispInt(cs);
	DispColorStr("EIP:", text_color);
	DispInt(eip);

	if(errCode != 0xFFFFFFFF){
		DispColorStr("Error code:", text_color);
		DispInt(errCode);
	}
}

int k = 0x1;
void HardwareInt(int irqNo)
{
    DispStr("spurious_irq: ");
    DispInt(k);
	k++;
    DispStr("\n");
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
