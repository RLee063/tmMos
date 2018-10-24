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

