#include "kernel.h"
#include "funcs.h"
#include "type.h"
#include "protect.h"
//==================================================================================
PROCESS* pidToPPro(int pid){
    return &procTable[pid];
}

int pProcToPid(PROCESS * pP){
    return pP - procTable;
}

void block(PROCESS * pP){
    schedule();
}

void unblock(PROCESS * pP){
    pP->status = NORMAL;
}
//=================================================================================

void sendMessage(int dest, MESSAGE* pMsg, PROCESS* current){
    current->pMsg = pMsg;
    PROCESS * pDest = pidToPPro(dest);
    if((pDest->status == RECEIVING)&&((pDest->receiveFrom == pProcToPid(current))||pDest->receiveFrom == ANY)){
 //       printf("s1");
        MemCpy(pDest->pMsg, current->pMsg, sizeof(MESSAGE));
        unblock(pDest);
        return;
    }
    else{
        pDest->senderQueue = current; 
        current->sendTo = dest;
        current->status = SENDING;
        block(current);
//        printf("s2");
    }
}

void receiveMessage(int src, MESSAGE* pMsg, PROCESS* current){
    current->pMsg = pMsg;
    if(src==ANY){
        if((current->senderQueue)&&(current->senderQueue->status==SENDING)){
            MemCpy(current->pMsg, current->senderQueue->pMsg, sizeof(MESSAGE));
            unblock(current->senderQueue);
            current->senderQueue = 0;
 //           printf("r1");
        }
        else{
            current->status = RECEIVING;
            current->receiveFrom = ANY;
            block(current);
 //           printf("r2");
        }
    }
    else{
        PROCESS * pSrc = pidToPPro(src);
        if((pSrc->status==SENDING)&&((pSrc->sendTo==ANY)||(pSrc->sendTo==pProcToPid(current)))){
            MemCpy(current->pMsg, pSrc->pMsg, sizeof(MESSAGE));
            unblock(pSrc);
 //           printf("r3");
        }
        else{
            current->receiveFrom = src;
            current->status = RECEIVING;
            block(current); 
//            printf("r4");
        }
    }
}

int syscallSendRecv(FUNCTION function, int srcDest, MESSAGE* pMsg, PROCESS* current){
    switch(function){
        case SEND:
            pMsg->source = pProcToPid(current);
            sendMessage(srcDest,  pMsg,  current);
            break;
        case RECEIVE:
            receiveMessage(srcDest,  pMsg,  current);
            break;
        default:
            break;
    }
    return 0;
}