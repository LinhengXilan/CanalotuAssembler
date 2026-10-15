; 功能：从根目录搜索 LOADER.BIN，加载到 1000:0000 后跳转

	org     0x7C00

; ============================================================
;  引导扇区入口
; ============================================================
	jmp     short Start
	nop

; ============================================================
;  BPB（标准 1.44MB FAT12 软盘）
; ============================================================
OEMName         db  "CANALOTU"
BPB_BytsPerSec  dw  512
BPB_SecPerClus  db  1
BPB_RsvdSecCnt  dw  1
BPB_NumFATs     db  2
BPB_RootEntCnt  dw  224
BPB_TotSec16    dw  2880
BPB_Media       db  0xF0
BPB_FATSz16     dw  9
BPB_SecPerTrk   dw  18
BPB_NumHeads    dw  2
BPB_HiddSec     dd  0
BPB_TotSec32    dd  0
BS_DrvNum       db  0
BS_Reserved1    db  0
BS_BootSig      db  0x29
BS_VolID        dd  0
BS_VolLab       db  "NO NAME    "
BS_FilSysType   db  "FAT12   "

; ============================================================
;  常量
; ============================================================
SECTOR_NUM_OF_ROOT_DIR  equ 19          ; 1 + 9 + 9
SECTOR_NUM_OF_DATA      equ 33          ; 1 + 9 + 9 + 14
ROOT_DIR_SECTORS        equ 14
SECTORS_PER_FAT         equ 9
ROOT_ENTRIES            equ 224
SECTORS_PER_TRACK       equ 18
HEADS                   equ 2

BUFFER_OFFSET           equ 0x8000      ; 目录 / FAT 的临时缓冲区
LOADER_BASE             equ 0x1000      ; loader 加载段
LOADER_OFFSET           equ 0x0000

; ============================================================
;  入口
; ============================================================
Start:
	cli
	mov     ax, cs
	mov     ds, ax
	mov     es, ax
	mov     ss, ax
	mov     sp, 0x7C00
	sti

	mov     [bootDrive], dl              ; BIOS 传入的驱动器号

	mov     si, msgLoading
	call    PrintString

; ============================================================
;  读根目录到 BUFFER_OFFSET
; ============================================================
	mov     ax, SECTOR_NUM_OF_ROOT_DIR
	mov     cx, ROOT_DIR_SECTORS
	mov     bx, BUFFER_OFFSET
	call    ReadDisk

; ============================================================
;  在根目录里搜索 LOADER.BIN
; ============================================================
	mov     di, BUFFER_OFFSET
	mov     cx, ROOT_ENTRIES
SearchLoop:
	push    cx
	push    di                          ; 保存条目起始位置
	mov     cx, 11
	mov     si, loaderName
	cld
	repe    cmpsb
	pop     di
	pop     cx
	jz      Found
	add     di, 32                      ; 下一个目录项
	loop    SearchLoop

	mov     si, msgNotFound
	call    PrintString
	jmp     $

; ============================================================
;  找到 LOADER.BIN
; ============================================================
Found:
	; DI 指向目录项起始
	mov     ax, [di + 26]               ; 起始簇号
	mov     [clusterNo], ax

; 读 FAT1（9 个扇区）到 BUFFER_OFFSET
	mov     ax, 1
	mov     cx, SECTORS_PER_FAT
	mov     bx, BUFFER_OFFSET
	call    ReadDisk

; ============================================================
;  加载文件簇链
; ============================================================
	mov     ax, LOADER_BASE
	mov     [loaderSeg], ax

LoadLoop:
	; LBA = SECTOR_NUM_OF_DATA + (clusterNo - 2)
	mov     ax, [clusterNo]
	sub     ax, 2
	add     ax, SECTOR_NUM_OF_DATA

	; 读 1 个扇区到 loaderSeg:0
	mov     es, [loaderSeg]
	mov     bx, LOADER_OFFSET
	mov     cx, 1
	call    ReadDisk

	; 前进 512 字节 = 0x20 段
	mov     ax, [loaderSeg]
	add     ax, 0x20
	mov     [loaderSeg], ax

	; 取下一个簇号
	mov     ax, [clusterNo]
	call    GetNextCluster
	mov     [clusterNo], ax
	cmp     ax, 0xFFF
	jb      LoadLoop

; ============================================================
;  跳转到 loader
; ============================================================
	jmp     LOADER_BASE:LOADER_OFFSET

; ============================================================
;  PrintString —— 打印以 0 结尾的字符串
;  输入：DS:SI
; ============================================================
PrintString:
	push    ax
	push    bx
	push    si
	mov     ah, 0x0E
	mov     bx, 0x0007
.Loop:
	lodsb
	test    al, al
	jz      .Done
	int     0x10
	jmp     .Loop
.Done:
	pop     si
	pop     bx
	pop     ax
	ret

; ============================================================
;  ReadDisk —— 读扇区（INT 13h AH=02h，CHS 寻址）
;
;  输入：AX = 起始 LBA
;        CX = 扇区数
;        ES:BX = 缓冲区
;
;  LBA → CHS：
;    柱面 C = LBA / (18 × 2)
;    磁头 H = (LBA / 18) % 2
;    扇区 S = (LBA % 18) + 1
; ============================================================
ReadDisk:
	push    ax
	push    bx
	push    cx
	push    dx
	push    si
	push    bp
	push    es

	mov     bp, cx                      ; 剩余扇区数

.NextSector:
	; LBA → CHS
	push    ax
	xor     dx, dx
	mov     cx, SECTORS_PER_TRACK * HEADS
	div     cx
	mov     ch, al                      ; CH = 柱面
	mov     ax, dx
	xor     dx, dx
	mov     cx, SECTORS_PER_TRACK
	div     cx
	mov     dh, al                      ; DH = 磁头
	mov     cl, dl
	inc     cl                          ; CL = 扇区号
	pop     ax

	; 读 1 个扇区
	push    ax
	mov     dl, [bootDrive]
	mov     ah, 0x02
	mov     al, 1
	int     0x13
	pop     ax
	jc      .DiskError

	inc     ax                          ; 下一个 LBA

	; 缓冲区 +512 字节
	add     bx, 512
	jnc     .NoCarry
	mov     si, es
	add     si, 0x20                    ; 512 / 16
	mov     es, si
.NoCarry:

	dec     bp
	jnz     .NextSector

	pop     es
	pop     bp
	pop     si
	pop     dx
	pop     cx
	pop     bx
	pop     ax
	ret

.DiskError:
	mov     si, msgDiskError
	call    PrintString
	jmp     $

; ============================================================
;  GetNextCluster —— 取 FAT12 表项
;
;  输入：AX = 当前簇号
;  输出：AX = 下一个簇号
; ============================================================
GetNextCluster:
	mov     bx, ax                      ; 保存簇号（用于判断奇偶）

	; offset = (cluster * 3) / 2
	mov     dx, 0
	mov     cx, 3
	mul     cx                          ; DX:AX = cluster × 3
	mov     cx, 2
	div     cx                          ; AX = offset

	mov     si, BUFFER_OFFSET
	add     si, ax
	mov     ax, [si]                    ; 读 16 位

	test    bx, 1
	jz      .Even
	shr     ax, 4                       ; 奇数项：高 12 位
	jmp     .Done
.Even:
	and     ax, 0x0FFF                  ; 偶数项：低 12 位
.Done:
	ret

; ============================================================
;  数据
; ============================================================
bootDrive       db  0
clusterNo       dw  0
loaderSeg       dw  0
loaderName      db  "LOADER  BIN"

msgLoading      db  "Loading LOADER.BIN...", 13, 10, 0
msgNotFound     db  "LOADER.BIN not found!", 13, 10, 0
msgDiskError    db  "Disk read error!", 13, 10, 0

; ============================================================
;  填充 + 引导签名
; ============================================================
	resb    510 - ($ - $$)
	dw      0xAA55