; hello-os
; TAB=4

        ORG		0x7c00          ; 指明程序的装载地址

; 以下的记述用于标准 FAT12 格式的软盘

        JMP		entry
        DB		0x90
        DB		"HELLOIPL"      ; 启动扇区名称，8 字节	
        DW		512				; 每个扇区（sector）大小，必须为 512 字节
        DB		1				; 簇（cluster）大小，必须为 1 个扇区
        DW		1				; FAT 起始位置，一般为第一个扇区
        DB		2				; FAT 个数，必须为 2
        DW		224				; 根目录大小，一般为 224 项
        DW		2880			; 该磁盘的大小，必须是 2880 扇区
        DB		0xf0			; 磁盘的种类，必须是 0xf0
        DW		9				; FAT 的长度，必须是 9 个扇区
        DW		18				; 一个磁道（track）有几个扇区，必须是 18
        DW		2				; 磁头数，必须是 2
        DD		0				; 不使用分区，必须是 0
        DD		2880			; 重写一次磁盘大小
        DB		0,0,0x29		; 意义不明，固定
        DD		0xffffffff		; （可能是）卷标号码
        DB		"HELLO-OS   "	; 磁盘的名称，必须为 11 字节
        DB		"FAT12   "		; 磁盘格式名称，必须为 8 字节
        RESB	18				; 空出 18 个字节

; 程序核心

entry:
		MOV		AX,0	        ; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
		MOV		DS,AX
		MOV		ES,AX

		MOV		SI,msg
putloop:
		MOV		AL,[SI]
		ADD		SI,1	        ; 给 SI 加 1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e	        ; 显示一个文字
		MOV		BX,15	        ; 指定字符颜色
		INT		0x10	        ; 调用显卡 bios
		JMP		putloop
fin:
		HLT				        ; 让 CPU 停止，等待指令
		JMP		fin		        ; 无限循环

msg:
		DB		0x0a, 0x0a      ; 换行 2 次
		DB		"hello, world"
		DB		0x0a	        ; 换行
		DB		0

		RESB	0x7dfe-$        ; 填写 0x00 直到 0x001fe

		DB		0x55, 0xaa

; 以下是启动区以外部分的输出

        DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	4600
		DB		0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
		RESB	1469432