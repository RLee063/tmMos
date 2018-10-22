#include "funcs.h"
int syscallGetTicks(){
    return ticks;
}

int syscallWrite(int unuse, char* buf, int length, PROCESS* pProc){
    // DispStr("length:");
    // DispInt(length);
    ttyWrite(&ttyTable[pProc->nr_tty], buf, length);
    return 0;
}
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
    PROCESS * pDest = pidToPPro(dest);
    if(pDest->status == RECEIVING&&(pDest->receiveFrom == pProcToPid(current||pDest->receiveFrom == ANY))){
        MemCpy(pDest->msg, current->msg, sizeof(MESSAGE));
        unblock(pDest);
        return;
    }
    else{
        pDest->senderQueue = current; 
        current->status = SENDING;
        block(current);
    }
}

void receiveMessage(int src, MESSAGE* pMsg, PROCESS* current){
    if(src==ANY){
        if(current->senderQueue&&current->senderQueue->status==SENDING){
            MemCpy(current->msg, current->senderQueue->msg, sizeof(MESSAGE));
            unblock(current->senderQueue);
            current->senderQueue = 0;
        }
        else{
            current->status = RECEIVING;
            block(current);
        }
    }
    else{
        PROCESS * pSrc = pidToPPro(src);
        if(pSrc->status==SENDING&&(pSrc->sendTo==ANY||pSrc->sendTo==pProcToPid(current))){
            MemCpy(current->msg, pSrc->msg, sizeof(MESSAGE));
            unblock(pSrc);
        }
    }
}

int syscallSendRecv(FUNCTION function, int srcDest, MESSAGE* pMsg, PROCESS* current){
    switch(function){
        case SEND:
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

void taskSyscall(){
    MESSAGE msg;
    printf("In the taskSyscall");
    while(1){
        SendRecv(RECEIVE, ANY, &msg);
        int src = msg.source;

        switch(msg.type){
            case GET_TICKS:
                msg.RETVAL = ticks;
                SendRecv(SEND, src, &msg);
                break;
        }
    }
}