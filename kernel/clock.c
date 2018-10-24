#include "type.h"
#include "funcs.h"
#include "kernel.h"
#include "protect.h"
#include "global.h"

void ClockClick(){
    ticks++;
    if(reEnterFlag!=0){
        return;
    }
    schedule();
}

void delay(int mSec){
    int t = GetTicks();
    while(((GetTicks()-t)*1000/HZ) < mSec){
    };
}