%ifdef READ_SECTOR
%else
%define READ_SECTOR
;startSectorNo in ax, numOfSectorToBeReaded in cl
;es: bx = where to put
%include    "FAT12.inc"
readSector:
    push    bp
    push    cx
    push    bx
    mov     bp, sp
    sub     esp, 2  ;TempStorage(numOfSectionToBeReaded)
    push    bx
    mov     byte [bp-2], cl
    mov     bl, [BPB_SecPerTrk]  ;18 = BPB_SecPerTrk
    div     bl
    inc     ah 
    mov     cl, ah  ;indexOfStartSection
    mov     dh, al  
    shr     al, 1   
    mov     ch, al  ;cylinder number
    and     dh, 1
    pop     bx
    mov     dl, [BS_DrvNum]   ;0 = [BS_DrvNum]
reRead:
    mov     ah, 2   ;read flag
    mov     al, byte [bp-2]  ;numToBeReaded
    int     13h
    jc      reRead ;if error redo read
    add     esp, 2
    pop     bx
    pop     cx
    pop     bp
    ret
%endif