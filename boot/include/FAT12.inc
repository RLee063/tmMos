%ifdef	FAT12_HDR
%else
%define	FAT12_HDR
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

secNoOfRootDirectory    equ     19      ;where to search  loader.com
secNoOfFstCluster       equ     17
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
%endif