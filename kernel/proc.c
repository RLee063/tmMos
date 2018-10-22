#include "type.h"
#include "funcs.h"
#include "kernel.h"
#include "protect.h"
#include "global.h"

void schedule(){
    while(1){
        nextProc++;
        if(nextProc>=procTable+NR_TASKS+NR_USER_PROCS){
            nextProc = procTable;
        }
        if(nextProc->status!=NORMAL){
            continue;
        }
        break;
    }
}

// void GetTicks(){
//     MESSAGE msg;
//     MemSet(&msg, 0, sizeof(MESSAGE));
//     msg.type = GET_TICKS;
//     SendRecv(SEND, TASK_SYS, &msg);
//     SendRecv(RECEIVE, TASK_SYS, &msg);
//     return msg.RETVAL;
// }