%ifdef	READ_FILE
%else
%define READ_FILE
;es = baseOfLoadAddress
;bx = offsetOfLoadAddress
;si = fileName
;cx = fileNameL

readFile:
    xor     ah, ah
    mov     dl, [BS_DrvNum]
    int     13h             ;replace the floppy driver
    readRootSectors:
        cmp     word [restOfRootSector], 0
        jz      foundFileFailed
        dec     word [restOfRootSector]
        push    cx
        mov     cl, 1
        mov     ax, [indexOfSectorToBeR]
        add     word [indexOfSectorToBeR], 1
        call    readSector
        pop     cx
        searchFile:
            cmp     word [restOfRootDir], 0
            jnz     goOnSearch
            mov     word [restOfRootDir], numOfDirInSector
            ;mov     word [offsetOfDirToBeCmp], offsetOfLoader
            mov     word [offsetOfDirToBeCmp], bx
            jmp     readRootSectors
        goOnSearch:
            dec     word [restOfRootDir]
            mov     di, [offsetOfDirToBeCmp]
            add     word [offsetOfDirToBeCmp], 32
            ;mov     si, loaderName
            push    si
            ;mov     cx, loaderNameL
            push    cx
            compareName:
                ;ds:si & es:di
                repe cmpsb
                jcxz    foundFileSuccess
                pop     cx
                pop     si
                jmp     searchFile

foundFileFailed:
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderNotFoundString
    mov     cx, loaderNotFoundStringL
    call    showMessage
    jmp     $

;di & 0xFFE0 = offset to loader
foundFileSuccess:
    pop     ax
    pop     ax  ;for the si and cx

    push    es
    mov     ax, ds
    mov     es, ax
    mov     ax, loaderFoundString
    mov     cx, loaderFoundStringL
    call    showMessage
    pop     es

    loadFile:
        and     di, 0xFFE0
        add     di, offsetOfDirFstClus
        mov     ax, [es:di]
        push    ax
        add     ax, secNoOfFstCluster
        add     ax, numOfRootDirSSector
        ;mov     bx, offsetOfLoader
        goOnLoad:
            mov     cl, 1
            call    readSector
            pop     ax
            call    getNextClusterNo
            cmp     ax, 0xFFF   
            jz      loadFileComplete
            push    ax
            add     ax, secNoOfFstCluster
            add     ax, numOfRootDirSSector
            add     bx, [BPB_BytesPerSec]
            jmp     goOnLoad
        loadFileComplete:
            ret          


;functions
getNextClusterNo:
    push    bx
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
    mov     ax, es
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
    oddCase:
        shr     ax, 4
    evenCase:
        and     ax, 0xFFF
    pop     es
    pop     bx
    ret

;string table
loaderFoundString   DB  'You found me!'
loaderFoundStringL  equ $-loaderFoundString
loaderNotFoundString    DB  'Im sorry, no such file here!'
loaderNotFoundStringL   equ $-loaderNotFoundString

;variable
restOfRootSector    DW  numOfRootDirSSector
restOfRootDir       DW  numOfDirInSector
indexOfSectorToBeR  DW  secNoOfRootDirectory
offsetOfDirToBeCmp  DW  0
oddFlag             DB  0

%endif