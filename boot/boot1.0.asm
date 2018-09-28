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

foundsLoader:
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

foundLoaderFailed:
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderNotFoundString
    mov     word cx, [loaderNotFoundStringL]
    call    showMessage
    jmp     $

;di & 0xFFE0 = offset to loader
foundLoaderSuccess:
    push    es
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderFoundString
    mov     word cx, [loaderFoundStringL]
    call    showMessage
    pop     es

    loadLoader:
        and     di, 0xFFE0
        add     di, offsetOfDirFstClus
        mov     ax, [es:di]
        push    ax
        add     ax, secNoOfFstCluster
        add     ax, numOfRootDirSSector
        mov     bx, offsetOfLoader
        goOnLoad:
            mov     cl, 1
            call    readSector
            pop     ax
            call    getNextClusterNo
            cmp     ax, 0xFFF   
            jz      loadLoaderComplete
            push    ax
            add     ax, secNoOfFstCluster
            add     ax, numOfRootDirSSector
            add     bx, [BPB_BytesPerSec]
            jmp     goOnLoad

loadLoaderComplete:
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderReady
    mov     word cx, [loaderReadyL]
    call    showMessage
    jmp     baseOfLoader:offsetOfLoader

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

;startSectorNo in ax, numOfSectorToBeReaded in cl
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

getNextClusterNo:
    push    es
    mov     bx, 3
    mul     bx
    mov     bx, 2
    div     bx
    mov     byte [oddFlag], dl
    ;ax = clusterNo
    mov     bx, [BPB_BytesPerSec]
    xor     dx, dx
    div     bx  ;dx = clusterIndex, ax = FATindex
    push    dx
    add     ax, sectorNoOfFAT1
    push    ax
    mov     ax, baseOfLoader
    sub     ax, 0x100
    mov     es, ax
    pop     ax
    mov     bx, 0
    mov     cl, 2
    call    readSector
    pop     di
    mov     word ax, [es:di]
    cmp     byte [oddFlag], 0
    jz  evenCase
    shr     ax, 4
    evenCase:
        and     ax, 0xFFF
    pop     es
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
loaderReady:        DB  'LOADER READY!'
loaderReadyL:       DW  13

;variable
restOfRootSector:   DW  numOfRootDirSSector
restOfRootDir:      DW  numOfDirInSector
indexOfSectorToBeR: DW  secNoOfRootDirectory
offsetOfDirToBeCmp: DW  offsetOfLoader
oddFlag:            DB  0

;restAndEndOfBoot
times 510-($-$$)    DB  0
DW  0xaa55               ;end of boot

;#define

%ifdef  DEBUG
baseOfStack     equ     0100h
%else
baseOfStack     equ     07c00h
%endif

secNoOfRootDirectory    equ     19      ;where to search  loader.com
secNoOfFstCluster       equ     17
baseOfLoader            equ     09000h   
offsetOfLoader          equ     0100h
numOfRootDirSSector     equ     14      ;
numOfDirInSector        equ     16
sectorNoOfFAT1          equ     1

;DIR

offsetOfDirName     equ     0
offsetOfDirAttr     equ     0xB
    ;reserve
offsetOfDirWrtTime  equ     0x16
offsetOfDirWrtDate  equ     0x18
offsetOfDirFstClus  equ     0x1A
offsetOfDirFileSIze equ     0x1C

        