#include "funcs.h"
int syscallGetTicks(){
    return ticks;
}

int syscallWrite(char* buf, int length, PROCESS* pProc){
    ttyWrite(&ttyTable[pProc->nr_tty], buf, length);
    return 0;
}