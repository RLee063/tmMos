#include "type.h"
#include "funcs.h"
#include "kernel.h"
#include "protect.h"
#include "global.h"

void ClockClick(){
    DispStr("#");
    if(reEnterFlag!=0){
        DispStr("!");
        return;
    }
    nextProc++;
    if(nextProc>=procTable+NR_TASKS){
        nextProc = procTable;
    }
}