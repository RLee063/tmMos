#include "kernel.h"
#include "funcs.h"
#include "type.h"
#include "protect.h"


//====================================================================
int GetTicks(){
    MESSAGE msg;
    MemSet(&msg, 0, sizeof(msg));
    msg.type = GET_TICKS;
    SendRecv(SEND, TASK_SYS, &msg);
	// printf("GetTicks() send ok! /");
    SendRecv(RECEIVE, TASK_SYS, &msg);
	// printf("GetTIcks(): return = %d/", msg.RETVAL);
    return msg.RETVAL;
}
//====================================================================

int syscallGetTicks(){
    return ticks;
}

int syscallPrintx(int unuse, int unuse2, char* s, PROCESS* pProc){
    const char * p;
	char ch;

	char reenter_err[] = "? k_reenter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;
	if (reEnterFlag == 0)  /* printx() called in Ring<1~3> */
		p = va2la(pProcToPid(pProc), s);
	else if (reEnterFlag > 0) /* printx() called in Ring<0> */
		p = s;
	else	/* this should NOT happen */
		p = reenter_err;
    if(*p == MAG_CH_ASSERT){
    }
	if ((*p == MAG_CH_PANIC) ||
	    (*p == MAG_CH_ASSERT && nextProc < &procTable[NR_TASKS])) {
		DisableInt();
		char * v = (char*)V_MEM_BASE;
		const char * q = p + 1; /* +1: skip the magic char */

		while (v < (char*)(V_MEM_BASE + V_MEM_SIZE)) {
			*v++ = *q++;
			*v++ = RED_CHAR;
			if (!*q) {
				while (((int)v - V_MEM_BASE) % (SCREEN_WIDTH * 16)) {
					/* *v++ = ' '; */
					v++;
					*v++ = GRAY_CHAR;
				}
				q = p + 1;
			}
		}

		__asm__ ("hlt");
	}

	while ((ch = *p++) != 0) {
		if (ch == MAG_CH_PANIC || ch == MAG_CH_ASSERT)
			continue; /* skip the magic char */

		out_char(ttyTable[pProc->nr_tty].p_console, ch);
	}
	return 0;
}


void taskSyscall(){
    MESSAGE msg;
    while(1){
        SendRecv(RECEIVE, ANY, &msg);
        int src = msg.source;
		// printf("taskSyscall() receive msg.type = %d \n", msg.type);
		// printf("taskSyscall() receive msg.source = %d \n", msg.source);
        switch(msg.type){
            case GET_TICKS:
                msg.RETVAL = ticks;
                SendRecv(SEND, src, &msg);
				// printf("taskSyscall() send ok!/");
                break;
        }
    }
}