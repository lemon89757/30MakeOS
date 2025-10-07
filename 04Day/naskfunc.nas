; naskfunc
; TAB=4

[FORMAT "WCOFF"]                ; 制作目标文件的模式
[INSTRSET "i486p"]              ; 使用到 486 为止的指令
[BITS 32]                       ; 制作 32 位模式用的机器语言
[FILE "naskfunc.nas"]           ; 文件名

        GLOBAL  _io_hlt, _write_mem8

[SECTION .text]

_io_hlt:                        ; void io_hlt(void);
        HLT
        RET

_write_mem8:                    ; void write_mem8(int addr, int data)
        MOV     ECX, [ESP + 4]
        MOV     AL, [ESP + 8]
        MOV     [ECX], AL
        RET