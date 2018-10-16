#include "type.h"
#include "funcs.h"
#include "kernel.h"
#include "protect.h"
#include "global.h"

void ClockClick(){
    ticks++;
    //DispStr("#");
    if(reEnterFlag!=0){
        //DispStr("!");
        return;
    }
    nextProc++;
    if(nextProc>=procTable+NR_TASKS+NR_USER_PROCS){
        nextProc = procTable;
    }
}

void delay(int mSec){
    int t = GetTicks();
    while(((GetTicks()-t)*1000/HZ) < mSec){
    };
}