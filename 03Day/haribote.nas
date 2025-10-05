; haribote-os
; TAB=4

; BOOT_INFO
CYLS    EQU     0x0ff0                  ; 引导扇区设置
LEDS    EQU     0x0ff1      
VMODE   EQU     0x0ff2                  ; 关于颜色的信息
SCRNX   EQU     0x0ff4                  ; 分辨率 x
SCRNY   EQU     0x0ff6                  ; 分辨率 y
VRAM    EQU     0x0ff8                  ; 图像缓冲区的起始地址

        ORG     0xc200                  ; 该程序将要被装载到的内存位置

        MOV     AL, 0x13                ; VGA 显卡，320 x 200 x 8 位彩色
        MOV     AH, 0x00
        INT     0x10
        MOV     BYTE [VMODE], 8         ; 屏幕的模式
        MOV     WORD [SCRNX], 320
        MOV     WORD [SCRNY], 200
        MOV     DWORD [VRAM], 0x000a0000

; 通过 BIOS 获取指示灯状态

        MOV     AH, 0x02
        INT     0x16            ; keyboard BIOS
        MOV     [LEDS], AL

fin:
        HLT
        JMP     fin
