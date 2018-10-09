#include "type.h"
#include "kernel.h"
#include "protect.h"
#include "funcs.h"
#include "global.h"

void initSysCallTable(){
    sysCallTable[0] = syscallGetTicks;
}

void initClock(){
	Out(TIMER_MODE, RATE_GENERATOR);
	Out(TIMER0, (u8) (TIMER_FREQ/HZ) );
	Out(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));
	irqTable[0] = ClockClick;
	EnableIrq(CLOCK_IRQ);
}

void initTaskTable(){
	taskTable[0].initial_eip = TestA;
	taskTable[0].stacksize = STACK_SIZE_TESTA;
	StrCpy(taskTable[0].name, "TESTA");
	taskTable[1].initial_eip = TestB;
	taskTable[1].stacksize = STACK_SIZE_TESTB;
	StrCpy(taskTable[1].name, "TESTB");
	taskTable[2].initial_eip = taskTty;
	taskTable[2].stacksize = STACK_SIZE_TESTC;
	StrCpy(taskTable[2].name, "TESTC");
}

void initKeyboard(){
	keyboardInput.pHead = keyboardInput.pTail = keyboardInput.buf;
	keyboardInput.count = 0;
    irqTable[1] = KeyboardHandler;
	EnableIrq(KEYBOARD_IRQ);
}

void initProcTable(){
	PROCESS * pProc = procTable;
    TASK * pTask = taskTable;
    u16 selectorLdt = SELECTOR_LDT_FIRST;
    char * pTaskStack = taskStack;

    for(int i=0; i<NR_TASKS; i++){
        StrCpy(pProc->p_name, pTask->name);
        pProc->pid = i;
        pProc->ldt_sel = selectorLdt;

        MemCpy(&pProc->ldts[0], &gdt[SELECTOR_KERNEL_CS>>3], sizeof(DESCRIPTOR));
        pProc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
        MemCpy(&pProc->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(DESCRIPTOR));
        pProc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;

        pProc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		pProc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		pProc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		pProc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		pProc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		pProc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

        pProc->regs.eip = (u32)pTask->initial_eip;
        pProc->regs.esp = (u32)pTaskStack;
        pProc->regs.eflags = 0x1202;

        pTaskStack -= pTask->stacksize;
        pProc ++;
        pTask ++;
        selectorLdt += (1<<3);
    }
    reEnterFlag = 0;
    nextProc = procTable;
}

int KernelMain()
{
	DispStr("----------------\"kernelMain\"----");
	initSysCallTable();
	initTaskTable();
	initKeyboard();
	initClock();
	initProcTable();
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
	int i = 0x12345;
	
	while(1){
		//DispStr("A");
		//DispInt(i++);
		//DispStr(".");
		delay(1000);
	}
}

void TestB()
{
	int i = 0x1000;
	while(1){
		//DispStr("B");
		//DispInt(i++);
		//DispStr(".");
		//delay(1000);
	}
}

void TestC()
{
	int i = 0x1000;
	while(1){
		//DispStr("C");
		//DispInt(i++);
		//DispStr(".");
		delay(1000);
	}
}

//===========temp funcs===================