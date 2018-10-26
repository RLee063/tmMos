#include "kernel.h"
#include "funcs.h"
#include "type.h"
#include "protect.h"
//==================================================================================
PROCESS *pidToPPro(int pid)
{
    return &procTable[pid];
}

int pProcToPid(PROCESS *pP)
{
    return pP - procTable;
}

void block(PROCESS *pP)
{
    schedule();
}

void unblock(PROCESS *pP)
{
    pP->status = NORMAL;
}
//=================================================================================

void sendMessage(int dest, MESSAGE *pMsg, PROCESS *current)
{
    assert(pProcToPid(current) != dest);

    //检查死锁
    int currentPid = pProcToPid(current);
    current->pMsg = pMsg;
    PROCESS *pDest = pidToPPro(dest);

    if ((pDest->status == RECEIVING) && ((pDest->receiveFrom == currentPid || pDest->receiveFrom == ANY)))
    {
        //       printf("s1");
        MemCpy(va2la(dest, pDest->pMsg), va2la(currentPid, current->pMsg), sizeof(MESSAGE));
        pDest->receiveFrom = NO_TASK;
        unblock(pDest);
        return;
    }
    else
    {
        current->sendTo = dest;
        current->status = SENDING;
        PROCESS *p;
        if (pDest->senderQueue)
        {
            p = pDest->senderQueue;
            while (p->nextSender)
            {
                p = p->nextSender;
            }
            p->nextSender = current;
        }
        else
        {
            pDest->senderQueue = current;
        }
        assert(current->nextSender == 0);
        //        printf("s2");
        block(current);
        return;
    }
}

void receiveMessage(int src, MESSAGE *pMsg, PROCESS *current)
{
    PROCESS *pSrc = pidToPPro(src);
    current->pMsg = pMsg;
    int hasMessage = 0;
    if (src == ANY)
    {
        if ((current->senderQueue))
        {
            hasMessage = 1;
            MemCpy(current->pMsg, current->senderQueue->pMsg, sizeof(MESSAGE));
            current->senderQueue = current->senderQueue->nextSender;
            current->senderQueue->sendTo = NO_TASK;
            current->senderQueue->nextSender = 0;
            unblock(current->senderQueue);
            //           printf("r1");
        }
    }
    else
    {
        PROCESS *pSrc = pidToPPro(src);
        if ((pSrc->status == SENDING) || (pSrc->sendTo == pProcToPid(current)))
        {
            hasMessage = 1;
            MemCpy(current->pMsg, pSrc->pMsg, sizeof(MESSAGE));

            PROCESS * p;
            PROCESS * pPre;
            if(current->senderQueue){
                if(current->senderQueue == pSrc){
                    current->senderQueue = pSrc->nextSender;
                }
                else{
                    pPre = current->senderQueue;
                    p = current->senderQueue->nextSender;
                    while((p->nextSender)&&(p!=pSrc)){
                        pPre = p;
                        p = p->nextSender;
                    }
                    if(p == pSrc){
                        pPre->nextSender = p->nextSender;
                    }
                }
            }

            pSrc->nextSender = 0;
            pSrc->sendTo = NO_TASK;
            unblock(pSrc);
            //           printf("r2");
        }
    }
    if (!hasMessage)
    {
        current->status = RECEIVING;
        current->receiveFrom = src;
        block(current);
        //printf("r3");
    }
}

int syscallSendRecv(FUNCTION function, int srcDest, MESSAGE *pMsg, PROCESS *current)
{
    switch (function)
    {
    case SEND:
        pMsg->source = pProcToPid(current);
        sendMessage(srcDest, pMsg, current);
        break;
    case RECEIVE:
        receiveMessage(srcDest, pMsg, current);
        break;
    default:
        break;
    }
    return 0;
}