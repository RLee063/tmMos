#include "funcs.h"
int syscallGetTicks(){
    return ticks;
}

int syscallPrintx(int unuse, int unuse2, char* s, PROCESS* pProc){
    // DispStr("length:");
    // DispInt(length);
    // ttyWrite(&ttyTable[pProc->nr_tty], buf, length);
    // return 0;
    //---old---
    const char * p;
	char ch;

	char reenter_err[] = "? k_reenter is incorrect for unknown reason";
	reenter_err[0] = MAG_CH_PANIC;
	/**
	 * @note Code in both Ring 0 and Ring 1~3 may invoke printx().
	 * If this happens in Ring 0, no linear-physical address mapping
	 * is needed.
	 *
	 * @attention The value of `k_reenter' is tricky here. When
	 *   -# printx() is called in Ring 0
	 *      - k_reenter > 0. When code in Ring 0 calls printx(),
	 *        an `interrupt re-enter' will occur (printx() generates
	 *        a software interrupt). Thus `k_reenter' will be increased
	 *        by `kernel.asm::save' and be greater than 0.
	 *   -# printx() is called in Ring 1~3
	 *      - k_reenter == 0.
	 */
	if (reEnterFlag == 0)  /* printx() called in Ring<1~3> */
		p = va2la(pProcToPid(pProc), s);
	else if (reEnterFlag > 0) /* printx() called in Ring<0> */
		p = s;
	else	/* this should NOT happen */
		p = reenter_err;

	/**
	 * @note if assertion fails in any TASK, the system will be halted;
	 * if it fails in a USER PROC, it'll return like any normal syscall
	 * does.
	 */
    if(*p == MAG_CH_ASSERT){
        DispStr("MAG_CH_ASSERT\n");
    }
	if ((*p == MAG_CH_PANIC) ||
	    (*p == MAG_CH_ASSERT && nextProc < &procTable[NR_TASKS])) {
        DispStr("SYSTEM_ASSERT\n");
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
    DispStr("SYSCALL_PRINT_RETURN\n");
	return 0;
}
//==================================================================================
PROCESS* pidToPPro(int pid){
    return &procTable[pid];
}

int pProcToPid(PROCESS * pP){
    return pP - procTable;
}

void block(PROCESS * pP){
    schedule();
}

void unblock(PROCESS * pP){
    pP->status = NORMAL;
}
//=================================================================================

void sendMessage(int dest, MESSAGE* pMsg, PROCESS* current){
    PROCESS * pDest = pidToPPro(dest);
    if(pDest->status == RECEIVING&&(pDest->receiveFrom == pProcToPid(current||pDest->receiveFrom == ANY))){
        MemCpy(pDest->msg, current->msg, sizeof(MESSAGE));
        unblock(pDest);
        return;
    }
    else{
        pDest->senderQueue = current; 
        current->status = SENDING;
        block(current);
    }
}

void receiveMessage(int src, MESSAGE* pMsg, PROCESS* current){
    if(src==ANY){
        if(current->senderQueue&&current->senderQueue->status==SENDING){
            MemCpy(current->msg, current->senderQueue->msg, sizeof(MESSAGE));
            unblock(current->senderQueue);
            current->senderQueue = 0;
        }
        else{
            current->status = RECEIVING;
            block(current);
        }
    }
    else{
        PROCESS * pSrc = pidToPPro(src);
        if(pSrc->status==SENDING&&(pSrc->sendTo==ANY||pSrc->sendTo==pProcToPid(current))){
            MemCpy(current->msg, pSrc->msg, sizeof(MESSAGE));
            unblock(pSrc);
        }
    }
}

int syscallSendRecv(FUNCTION function, int srcDest, MESSAGE* pMsg, PROCESS* current){
    switch(function){
        case SEND:
            sendMessage(srcDest,  pMsg,  current);
            break;
        case RECEIVE:
            receiveMessage(srcDest,  pMsg,  current);
            break;
        default:
            break;
    }
    return 0;
}

void taskSyscall(){
    MESSAGE msg;
    while(1){}
    // while(1){
    //     SendRecv(RECEIVE, ANY, &msg);
    //     int src = msg.source;

    //     switch(msg.type){
    //         case GET_TICKS:
    //             msg.RETVAL = ticks;
    //             SendRecv(SEND, src, &msg);
    //             break;
    //     }
    // }
}