#pragma once
#include "global.h"
#include "type.h"
//extern
void MemCpy(void *pDst, void *pSrc, u32 size);
void MemSet(void *pDst, char ch, int size);
void DispStr(void *pStr);
void Out(u32 port, u32 val);
void In(u32 port);
void StrCpy(void *pDst, void* pSrc);
void ClockClick();
int KernelMain();
void DispColorStr(void *pStr, int color);


void restart();
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();
//global
void CpuInt(int vecNo, int errCode, int eip, int cs, int eflags);
void HardwareInt(int irqNo);
//private
void init8259A();
void DispInt(int input);
void initIdt();
void TestA();
void TestB();
