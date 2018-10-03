#include "proto.h"

void init_8259A(){
    //ICW1
    Out(INT_M_CTL, 0x11);
    Out(INT_S_CTL, 0x11);
    //ICW2
    Out(INT_M_CTLMASK, INT_VECTOR_IRQ0);
    Out(INT_S_CTLMASK, INT_VECTOR_IRQ8);
    //ICW3
    Out(INT_M_CTLMASK, 0x4);
    Out(INT_S_CTLMASK, 0x2);
    //ICW4
    Out(INT_M_CTLMASK, 0x1);
    Out(INT_S_CTLMASK, 0x1);
    //OCW1
    Out(INT_M_CTLMASK, 0xFD);
    Out(INT_S_CTLMASK, 0xFF);
}
