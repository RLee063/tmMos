#include "type.h"
#include "kernel.h"
#include "protect.h"
#include "funcs.h"
#include "global.h"

void initSysCallTable(){
    sysCallTable[NR_GetTicks] = syscallGetTicks;
	sysCallTable[NR_Write] = syscallWrite;
	sysCallTable[NR_SendRecv] = syscallSendRecv;
}

void initClock(){
	Out(TIMER_MODE, RATE_GENERATOR);
	Out(TIMER0, (u8) (TIMER_FREQ/HZ) );
	Out(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));
	irqTable[0] = ClockClick;
	EnableIrq(CLOCK_IRQ);
}

void initProcTable(){
	PROCESS * pProc = procTable;
    TASK * pTask;
    u16 selectorLdt = SELECTOR_LDT_FIRST;
    char * pTaskStack = taskStack + STACK_SIZE_TOTAL;
	u8 privilege;
	u8 rpl;
	int eflags;
    for(int i=0; i<NR_TASKS + NR_USER_PROCS; i++){
		if(i<NR_TASKS){
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202;
			pTask = taskTable + i;
		}
		else{
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202;
			pTask = userProcTable + (i - NR_TASKS);
		}
        StrCpy(pProc->p_name, pTask->name);
        pProc->pid = i;
        pProc->ldt_sel = selectorLdt;

        MemCpy(&pProc->ldts[0], &gdt[SELECTOR_KERNEL_CS>>3], sizeof(DESCRIPTOR));
        pProc->ldts[0].attr1 = DA_C | privilege << 5;
        MemCpy(&pProc->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(DESCRIPTOR));
        pProc->ldts[1].attr1 = DA_DRW | privilege << 5;

        pProc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		pProc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		pProc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		pProc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		pProc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		pProc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| rpl;

        pProc->regs.eip = (u32)pTask->initial_eip;
        pProc->regs.esp = (u32)pTaskStack;
        pProc->regs.eflags = eflags;

		pProc->nr_tty = 0;
        pTaskStack -= pTask->stacksize;
		pProc->status = NORMAL;

        pProc ++;
        selectorLdt += (1<<3);
    }
	//=====
	procTable[1].nr_tty = 1;
	procTable[2].nr_tty = 2;
	//======
    reEnterFlag = 0;
    nextProc = procTable;
}

int KernelMain()
{
	DispStr("----------------\"kernelMain\"----\n");
	initSysCallTable();
	DispStr("SysCallTable complete!\n");
	DispInt(&DispPos);
	DispStr(":&disPos\n");
	DispInt(taskTty);
	DispStr(":taskTty\n");
	initClock();
	DispStr("Clock complete!\n");
	initProcTable();
	DispStr("ProcTable complete!\n");
    restart();
    while(1){
		DispStr("你永远见不到我");
	}    
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	while(1){
		printf("hello?");
		delay(3000);
	}
}

void TestB()
{
	int i = 0x1086;
	while(1){
		printf("%x", i);
		delay(3000);
	}
}

void TestC()
{
	while(1){
		delay(3000);
	}
}

//===========temp funcs===================