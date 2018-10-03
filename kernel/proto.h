#include "type.h"
#include "const.h"
#include "protect.h"

void MemCpy(void* pDst, void* pSrc, u32 size);
void DispStr(void* pStr);
void Out(u32 port, u32 val);
void In(u32 port, u32 val);