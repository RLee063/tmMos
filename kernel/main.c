#include "type.h"
#include "kernel.h"
#include "protect.h"
#include "funcs.h"
#include "global.h"

int KernelMain()
{
    DispStr("----------------\"kernelMain\"----");
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
    reEnterFlag = -1;
    nextProc = procTable;
    restart();

    while(1){}    

}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int i = 0x12345;
	
	while(1){
		DispStr("A");
		DispInt(i++);
		DispStr(".");
		for(int i=0; i<9; i++){
			for(int j=0; j<9999; j++){

			}
		}
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0x1000;
	while(1){
		DispStr("B");
		DispInt(i++);
		DispStr(".");
		//delay(1);
	}
}
