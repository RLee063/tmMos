;%define DEBUG
%ifdef DEBUG
    org     0100h
%else
    org     7c00h
%endif

; BS_jmpBoot
    jmp short bootStart
    nop

BS_OEMName          DB      'ToHisMom'
BPB_BytesPerSec     DW      512
BPB_SecPerClus      DB      1
BPB_RsvdSecCnt      DW      1
BPB_NumFATs         DB      2
BPB_RootEntCnt      DW      224
BPB_TotSec16        DW      2880
BPB_Media           DB      0xF0
BPB_FATSz16         DW      9
BPB_SecPerTrk       DW      18
BPB_NumHeads        DW      2
BPB_HiddSec         DD      0
BPB_TotSec32        DD      0
BS_DrvNum           DB      0
BS_Reserved1        DB      0
Bs_RootSig          DB      29h
BS_VolID            DD      0
BS_VolLab           DB      'ToMyMomSOSL'
BS_FileSysType      DB      'FAT12'

bootStart:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     esp, baseOfStack
    mov     ax, bootString
    mov     word cx, [bootStringL]
    call    showMessage

    xor     ah, ah
    mov     dl, [BS_DrvNum]
    int     13h             ;replace the floppy driver

loadLoader:
    readRootSectors:
        cmp     word [restOfRootSector], 0
        jz      foundLoaderFailed
        dec     word [restOfRootSector]
        mov     cl, 1
        mov     ax, baseOfLoader
        mov     es, ax
        mov     bx, offsetOfLoader
        mov     ax, [indexOfSectorToBeR]
        add     word [indexOfSectorToBeR], 1
        call    readSector
        searchLoader:
            cmp     word [restOfRootDir], 0
            jnz     goOnSearch
            mov     word [restOfRootDir], numOfDirInSector
            mov     word [offsetOfDirToBeCmp], offsetOfLoader
            jmp     readRootSectors
        goOnSearch:    
            dec     word [restOfRootDir]
            mov     ax, baseOfLoader
            mov     es, ax
            mov     di, [offsetOfDirToBeCmp]
            add     word [offsetOfDirToBeCmp], 32
            mov     si, loaderName
            mov     cx, [loaderNameL]
            compareName:
                repe cmpsb
                jcxz    foundLoaderSuccess
                jmp     searchLoader

foundLoaderSuccess:
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderFoundString
    mov     word cx, [loaderFoundStringL]
    call    showMessage
    jmp     $

foundLoaderFailed:
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderNotFoundString
    mov     word cx, [loaderNotFoundStringL]
    call    showMessage
    jmp     $

;ax = address(string)
;cx = length(string)
showMessage:
    ;mov     ax, bootString
    mov     bp, ax          ;ES:BP string's address
    ;mov     cx, 24          ;CX: length(string)
    mov     ax, 01301h      ;AH 13h, AL 01h
    mov     bx, 000ch       ;BH: numOfPage, BL=0ch: highlight
    mov     dl, 0
    int     10h
    ret

;numOfSector in ax, numOfSectorToBeReaded in cl
;es & bx are params
readSector:
    push    bp
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
    pop     bp
    ret

;string table
bootString:         DB  'Hello! This is tmMos OS!'
bootStringL:        DW  24
loaderFoundString:  DB  'You found me!'
loaderFoundStringL: DW  13
loaderNotFoundString:   DB  'Im sorry, no loader here!'
loaderNotFoundStringL:  DW  26
loaderName:         DB  'LOADER0 COM'
loaderNameL:        DW  11

;variable
restOfRootSector:   DW  numOfRootDirSSector
restOfRootDir:      DW  numOfDirInSector
indexOfSectorToBeR: DW  secNoOfRootDirectory
offsetOfDirToBeCmp: DW  offsetOfLoader

;restAndEndOfBoot
times 510-($-$$)    DB  0
DW  0xaa55               ;end of boot



%ifdef  DEBUG
baseOfStack     equ     0100h
%else
baseOfStack     equ     07c00h
%endif

secNoOfRootDirectory    equ     19      ;where to search  loader.com
baseOfLoader            equ     09000h   
offsetOfLoader          equ     0100h
numOfRootDirSSector     equ     14      ;
numOfDirInSector        equ     16

        