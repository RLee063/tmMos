#pragma once
#include "global.h"
#include "type.h"
//temp
void delay(int mSec);
//Task
void taskTty();
void taskSyscall();
void TestA();
void TestB();
void TestC();
//global
int KernelMain();
void cstart();

//lib
#define ASSERT
#ifdef ASSERT
void assertion_failure(char *exp, char *file, char *base_file, int line);
#define assert(exp)  if (exp) ; \
        else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

#define printl printf

void MemCpy(void *pDst, void *pSrc, u32 size);
void MemSet(void *pDst, char ch, int size);
void DispStr(void *pStr);
void StrCpy(void *pDst, void* pSrc);
int StrLen(char* p_str);
void DispInt(int input);
void Out(u32 port, u32 val);
int In(u32 port);
void EnableIrq(int irq);
void DisableIrq(int irq);
void DispColorStr(void *pStr, int color);
void DisableInt();
void EnableInt();
//===================================================
//==========             extern         =============
//===================================================
void restart();
//CPU_INT
void CpuInt(int vecNo, int errCode, int eip, int cs, int eflags);

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
//Hardware_INT
void HardwareInt(int irqNo);

void ClockClick();
void hwint00();
void KeyboardHandler();
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
//Syscall
void SysCall();

int GetTicks();
int syscallGetTicks();
int printx(char*);
int syscallPrintx(int unuse, int unuse2, char* buf, PROCESS* pProc);
int SendRecv(FUNCTION function, int srcDest, MESSAGE* pMsg);
int syscallSendRecv(FUNCTION function, int srcDest, MESSAGE* pMsg, PROCESS* current);
