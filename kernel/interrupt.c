#include "type.h"
#include "funcs.h"
#include "kernel.h"
#include "protect.h"
#include "global.h"

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

void HardwareInt(int irqNo)
{
    DispStr("spurious_irq: ");
    DispInt(irqNo);
    DispStr("\n");
}
