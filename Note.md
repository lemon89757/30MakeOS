# Note for 30MakeOs
## references
1. [wiki.osdev.org](https://wiki.osdev.org/Expanded_Main_Page)
2. [BIOS](https://wiki.osdev.org/BIOS)
3. [Memory-Map X86](https://wiki.osdev.org/Memory_Map_(x86))

## 01 Day
### 二进制文件 `helloos.img`
- `helloos.img` 文件的一种简单生成方式：
  ```bash
  dd if=/dev/zero of=hello.img count=1474560 bs=1
  ```
  通过上述命令生成全为 0 的，大小为 1474560 `byte` 的二进制文件，再根据作者提示在二进制文件相应位置作更改即可；

- `helloos.img` 运行流程：
  ```bat
  <!-- 1. !cons_nt.bat -->
  cmd.exe

  <!-- 2. run.bat -->
  copy helloos.img ..\z_tools\qemu\fdimage0.bin
  ..\z_tools\make.exe -C ../z_tools/qemu

  <!-- 3. make.exe -->
  default:
        qemu-win.bat
  ...\make.exe: Entering directory D:/.../tolset/z_tools/qemu/qemu-win.bat

  <!-- 4. qemu-win.bat -->
  @set SDL_VIDEODRIVER=windib
  @set QEMU_AUDIO_DRV=none
  @set QEMU_AUDIO_LOG_TO_MONITOR=0
  qemu.exe -L . -m 32 -localtime -std-vga -fda fdimage0.bin
  ```
### 汇编文件 `helloos.nas`
- 汇编语法：
  - `;`: `;` 后的内容为注释内容；
  - `DB`: `define byte`，往文件里直接写入 1 个字节的指令。同时也可以用它来写字符串，这时汇编语言会自动地查找字符串中每个字符所对应的编码，然后一个一个字节地排列起来；
  - `DW`: `define word`，在计算机汇编语言的世界里，`word` 指的是 16 位的意思，即 2 个字节；
  - `DD`: `define double-word`，32 位，即 4 字节；
  - `RESB`: `reserve byte`，预留空间指令。`eg: RESB 10`，表示预留 10 个字节的空间；`RESB 0x1fe-$`，其中 `$` 表示一个变量，用以记录当前的字节数；
  - `ORG`: `Origin`，装载指令，指示将机器语言（当前文本内容）装载到内存中的哪个地址；此时，`$` 指令也不再表示输出文件的第几个字节，而是代表将要读入的内存地址；
  - `JMP`: 跳转指令（`JMP`）机器码 `EB`；
    - `JE`，条件跳转指令，如果前面的比较指令结果为相等，则转入 `JE` 进行跳转；如果不等，则不跳转，继续执行下一条指令；
    - `JC`，`jump if carry`，如果进位标志（`carry flag`）的话，就跳转；
    - `JNC`, `jump if not carry`，进位标志为 0 的话就跳转；
    - `JAE`, `jump if above or equal`，大于或等于时跳转；
    - `JB`, `jump if below`；
  - `MOV`: 赋值指令（转送指令）；`eg`: 
    - `MOV AX,0`、`MOV SS,AX`；
    - `MOV AL,[SI]`：读取 `SI` 的值所对应的内存地址并将读取结果赋给 `AL`）；
    - `MOV BYTE [678], 123`：用 `678` 号内存来保存 `123` 这个数值，`BYTE` 表示只占用一个字节内存；`MOV WORD [678], 123`，还是用 `678` 号内存来保存 `123` 这个数值，只不过这次需要使用 `WORD` 即 2 个字节内存，对应为 `678` 和 `679` 号内存。其中 `123d` 即 `0000 0000 0111 1011b` 的 `0000 0000` 保存在 `679` 号内存，剩余的 `0111 1011` 保存在 `678` 号内存；
    - `MOV` 指令有一个规则，就是源数据和目的数据必须相同位数。也就是说，能向 `AL` 里代入的就只有 `BYTE`，这样一来就可以省略 `BYTE`，即可以写成：`MOV AL, [SI]`；`MOV [0x1234], 0x56` 会出错，这是因为指定内存时，不知道到底是 `BYTE`，还是 `WORD`，还是 `DWORD`，**只有在另一方也是寄存器时才可以省略，其他情况下不能省略**；
  - `ADD`: 加法指令（演算指令），`ADD SI, 1` 即表示 `SI += 1`；
  - `CMP`: 比较指令，`CMP AL, 0`，表示如果二者相等或不等，需要做什么（后接条件跳转指令等）；
  - `INT`: 软件中断指令；
  - `HLT`: `halt`（停止），属于 I/O 指令，让 `CPU` 停止动作的指令，使 `CPU` 进入待机状态；在循环中可以避免 `CPU` 全力执行 `JMP` 指令，从而达到降低能耗的目的；
  - `EQU`: `equal`，可以用来声明常量，相当于 C 中的 `#define`；
  - `RET`: 相当于 C 语言中的 `return`；
  - `OUT`: CPU 与**设备**相连，需要向这些设备发送或获取数据；向设备发送数据即 `OUT` 指令；为了区分不同的设备，也要使用设备号码（`port`）；
  - `IN`: 从设备获取数据；
  - `CLI`: 将中断标志（`interrupt flag`）置 0 的指令（`clear interrupt flag`）；
  - `STI`：将中断标志置 1 的指令（`set interrupt flag`）；
- `asm.bat`
  ```cmd
  ..\z_tools\nask.exe helloos.nas helloos.img
  ```
  其中`nask` 是作者根据 `NASM` 开发的汇编语言编译器；

### 汇编文件加工润色 `helloReadable.nas`
- 汇编文件占 512 个字节，其中最后两个字节的内容为 `0x55 0xAA`（具体原因见后续启动区内容）；

### 扩展知识
#### 操作系统启动盘制作和 `QEMU`；
#### `TAB-4`
  - 表示 `TAB` 键的宽度为 4；
#### `FAT12` 格式
  - `FAT12 Format` 用 `Windows` 或 `MS-DOS` 格式化出来的软盘就是这种格式（文件系统内容）；
#### 启动区：
  - `boot sector`，**软盘的第一个扇区称为启动区**；**计算机首先从最初一个扇区开始读软盘，然后去检查这个扇区最后 2 个字节的内容。如果这最后 2 个字节的内容不是 `0x55 0xAA`，计算机会认为这张软盘上没有所需的启动程序，就会报一个不能启动的错误**；
  - 计算机读写软盘的时候并不是一个字节一个字节地读写，而是以 512 字节为单位进行读写。因此，软盘的 512 字节就称为一个扇区；
  - 一张软盘的空间共有 1440KB，也就是 1474560 字节，即 2880 个扇区；
#### `IPL`：
  - `initial program loader`，即启动程序加载器；
  - 启动区只有区区 512 字节，实际操作系统根本装不进去。所以几乎所有的操作系统都是把加载操作系统给本身的程序放在启动区里；因此，有时也将启动区称为 `IPL`；
  - 可以将 `HELLOIPL` 换成别的名字，但必须是 8 个字节长度；
#### `boot`
  - 即 `bootstrap`，操作系统自动启动机制；

## 02 Day
### 汇编文件进一步加工
```asm
; helloReadable02.nas
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
```
- **运行流程**（`x86` 实模式下的启动扇区（`boot sector`）程序）：
  1. 上电 -> `BIOS` 进行硬件自检（`POST`）；
  2. `BIOS` 搜索可启动设备（软盘、硬盘、U 盘等，软盘：个人电脑最早使用的移动存储介质，类似于 U 盘。软盘存取速度慢，容量也小，但可装卸、携带方便。目前普遍使用硬盘和 U 盘）;
  3. 如果发现设备第一个扇区（第 0 扇区，512 字节）最后两个字节的内容是 `0x55 0xAA`，就认为它是启动扇区；
  4. `BIOS` 将这 **512 字节**原样复制到内存地址 `0x7c00`（对应到上述代码中的 `ORG 0x7c00`）;
  5. 然后执行一条跳转指令（对应为 `JMP entry`），`CS:IP = 0000:7c00`，开始运行 `entry` 中的代码；
  6. `msg` 表示将要显示的字符串，在 `entry` 的结束时将其地址赋值给 `SI` 寄存器；
  7. `entry -> putloop -> putloop -> ...(read msg done, read '\0') -> fin`
  8. 在 `putloop` 中使用 `BIOS` 中断 `INT 0x10`(功能号 `AH=0x0E`)在屏幕上打印字符。其中 `AL` 表示要显示的字符，`BH` 表示页号，`BL` 表示颜色；
  9. 最后执行 `fin`，进入停机循环，`HLT` 指令让 `CPU` 暂停，等待外部中断（如键盘中断）。**`HLT` 避免空循环，节能**。
- 上述汇编代码的**其他说明**：
  - `BIOS` 把 512 字节读进内存 `0x7c00`，最后一步是“跳转到 `0:7c00`”，相当于 `JMP 0x0000:0x7c00(CS:IP)`, 从此处取第一条指令。而源码在 `0x7c00` 处正是 `JMP entry`，于是 `CPU` 立即跳到 `entry` 标号；
  - `0x0000 7c00 ~ 0x0000 7dff`，启动区内容的装载地址（规定）；
  - `entry` 即表示一个地址（偏移 `0x7c50` 左右），于是 `JMP entry` 为跳转到该地址开始执行程序(`entry:` 标签的声明。在汇编语言中，所有标签都仅仅是单纯的数字，每个标签对应的数字是由汇编语言编译器根据 `ORG` 命令计算出来的。编译器计算出的“标签的地方对应的内存地址”就是那个标签的值额值)；
  - `entry` 中的内容：初始化段寄存器，给 `SS, DS, ES` 初始化为 0，`SP` 初始化为 `0x7c00`，此时的内存布局大致为：
    ```bash
    0x00000+------------------+
          | 中断向量表 (IVT)  |
    0x00400+------------------+
          | BIOS 数据区       |
    0x07C00+------------------+ <-- 代码从这里开始
          | 启动扇区 (512B)   |
    0x07E00+------------------+
          | 空闲内存          |
    ```
  - 启动区以外的数据：`DB 0xf0, 0xff...`，它们不在 512 字节启动扇区范围内，`BIOS` 不会加载它们，也不会执行；**它们只是占位，让镜像文件符合软盘格式（`1.44MB = 2880` 扇区）**，方便写盘或虚拟机测试；
  - `JMP entry` 后的代码 `DB 0x90...` 永远都不会执行，它只是在引导扇区头 **0x50 字节**里占位置的数据，不是指令；从 `DB 0x90...`（从 `0x7c02 ~ 0x7c3d`）全部是 `FAT12` 引导参数（`OEM` 名（原始设备制造商名），扇区大小、`FAT` 表个数......），供系统识别；
  - `BIOS` 只检查最后两个字节是不是 `0x55 0xAA` 来决定这是不是启动扇区，一旦满足，它就把整个 512 字节原样搬进内存 `0x7c00` 并跳转，不解析也不关心前面 510 字节里放了什么数据；**“这是什么格式的软盘”——FAT12、FAT16、NTFS、ext，都是操作系统或引导程序自己的任务，从而决定怎么继续读盘**；换句话说，`BIOS` 只关心能否以该盘启动的问题；
  - `BIOS` 的工作到此为止：扇区末尾签名正确 -> 加载 -> 跳转；
  - 把 `SP` 设成 `0x7c00` 是典型的做法：
    - `todo`:程序加载后的内存分布：程序 + `BIOS` + 中断向量表 + `SP` 的允许移动的范围（结合上述内存布局）；
  - 上述 `SS:SP、CS:IP` 表示**16 位实模式下的段寄存器:偏移寄存器对**，**用来拼成 20 位物理地址**（因此寻址范围在 `1MB` 左右），**物理地址 = 段寄存器 × 16 + 偏移寄存器**：
    | 寄存器对                    | 名称       | 作用                 | 举例（值）               | 算出物理地址  |
    | ----------------------- | -------- | ------------------ | ------------------- | ------- |
    | **CS:IP**               | 代码段:指令指针 | 指出**下一条要执行的指令**在哪里 | CS=0x07C0 IP=0x0000 | 0x07C00 |
    | **SS:SP**               | 栈段:栈指针   | 指出**当前栈顶**在哪里      | SS=0x0000 SP=0x7C00 | 0x07C00 |
    | **DS:BX** 或 **DS:SI** 等 | 数据段:偏移   | 指出**数据**在哪里        | DS=0x0000 SI=0x7C50 | 0x07C50 |
    1. `CS:IP` 决定 `CPU` 从哪儿取指令；
    2. `SS:SP` 决定 `push/pop/call/ret` 时栈内存的位置；
    3. `DS/ES/FS/GS` 配合通用寄存器，用来访问数据。
    4. 在实模式里，所有内存访问都必须通过“**段:偏移**”这对组合来生成 **20 位地址**，最大寻址 **1 MB**（0x00000 ∼ 0xFFFFF）；**20 位是 16 位实模式的硬件限制**；**段:偏移→20 位**只是 16 位实模式的遗产；**现代 `CPU` 一上电先模拟这段历史，随后由操作系统切换到保护/长模式**，才打开真正的 `4 GB` 甚至更大地址空间（要访问 `4 GB` 需要进入 *32 位保护模式*（或 *64 位长模式*））；（`todo`，什么是实模式？）
    5. 保护模式下段寄存器不再当“段基址”用，而是索引 `GDT/LDT` 里的段描述符，描述符里放 32 位基址 + 32 位偏移 → 物理地址可达 4 GB（开启分页后更是线性地址 4 GB）。再往后 64 位长模式干脆把段基址固定为 0，直接用 64 位 `RIP` 寻址，理论 16 EB（实际 48-57 位地址线）
  - `PC`（程序计数器） 就是 `IP(Instruction Pointer, 16bits), EIP(Extended Instruction Pointer, 32bits), RIP(Register Instruction Pointer, 64bits)`：每个时钟周期 `CPU` 把 `CS:IP` 拼成物理地址送地址总线，同时把 `IP` 加上当前指令长度；始终保存着**下一条待执行指令**的偏移地址；
  - `INT 0x10`：调用中断号为 `0x10` 的中断（控制显卡）；后续的内容，以显示一个字符：`AH=0x0e`，`AL=character code`，`BH=0`，`BL=color code`；
  - 由 `helloReadable.nas -> helloReadable02.nas` 的转变：
    1. `helloReadable.nas` 中哪些内容对应到了 `helloReadable02.nas` 中的 `ORG 0x7c00`：`todo`；
    2. 前者的 `DB 0xeb, 0x4e` 怎么对应到 `JMP entry`：`JMP` 对应到 `0xeb`，但是 `entry` 不是对应到 `0x7c50`？`todo`；

### 启动区文件 `ipl.nas`
- 即 `helloReadable02.nas` 中除启动区以外的部分；

### 加入 `makefile`
- `Makefile`
  ```Makefile
  default:
    ../z_tools/make.exe img

  ipl.bin: ipl.nas Makefile
    ../z_tools/nask.exe ipl.nas ipl.bin ipl.lst

  helloos.img: ipl.bin Makefile
    ../z_tools/edimg.exe imgin:../z_tools/fdimg0at.tek \
    wbinimg src:ipl.bin len:512 from:0 to:0 imgout:helloos.img

  asm:
    ../z_tools/make.exe -r ipl.bin

  img:
    ../z_tools/make.exe -r helloos.img

  run:
    ../z_tools/make.exe img
    copy helloos.img ..\z_tools\qemu\fdimage0.bin
    ../z_tools/make.exe -C ../z_tools/qemu

  install:
    ../z_tools/make.exe img
    ../z_tools/imgtol.com w a: helloos.img

  clean:
    -del ipl.bin
    -del ipl.lst
  
  src_only:
    ../z_tools/make.exe clean
    -del helloos.img
  ```
  其中：
    - `ipl.lst` 文件为 `Assembler list file`，主要用来存储汇编程序列表数据。[编译工具链之三 ARM-MDK、IAR、GCC 的 .map 文件、.lst 文件](https://blog.csdn.net/zcshoucsdn/article/details/130449260)
    - `make -r`，即 `make -no-buildin-rules`, eliminate use of the build-in implicit rules.
    - `${MAKE} -C subdir`，等价于 `cd subdir && ${MAKE}`；`-C dir` 即 `--directory=dir`，change to directory `dir` before reading the makefiles. If multiple `-C` options are specified, each is interpreted relative to the previous one: `-C / -C etc` is equivalent to `-C /etc`.（`CC`, `C Compiler`）
- `make.bat`:
  ```bat
  ..\z_tools\make.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
  ```

### 扩展知识
#### `CPU` 中的寄存器
- 8  位寄存器
  - `AL`: 累加寄存器低位，`accumulator low`；
  - `CL`: 计数寄存器低位，`counter low`；
  - `DL`: 数据寄存器低位，`data low`；
  - `BL`: 基址寄存器低位，`base low`；
  - `AH`: 累加寄存器高位，`accumulator high`；
  - `CH`: 计数寄存器高位，`counter high`；
  - `DH`: 数据寄存器高位，`data high`；
  - `BH`: 基址寄存器高位，`base high`；
  1. `AL, AH` 分别表示 `AX` 的低 8 位和高 8 位；
  2. `BP, SP, SI, DI` 没有区分，如果想要对它们取高位或低位数据，就必须先用 `MOV AX, SI` 将 `SI` 的值赋给 `AX`，再用 `AL, AH` 来取值；
- 16 位寄存器
  - `AX`: `accumulator`，累加寄存器；
  - `CX`: `counter`，计数寄存器；
  - `DX`: `data`，数据寄存器
  - `BX`: `base`，基址寄存器；
  - `SP`: `stack pointer`，栈指针寄存器；
  - `BP`: `base pointer`，基址指针寄存器
  - `SI`: `source index`，源变址寄存器；
  - `DI`: `destination index`，目的变址寄存器；
  1. 在这 8 个寄存器中，不管使用哪一个，差不多都能进行同样的计算，但如果都用 `AX` 来进行各种运算的话，程序就可以写得很简洁：`ADD CX, 0x1234` 编译成 `81 C1 34 12`，是一个 4 字节命令，而 `AD AX, 0x1234` 编译成 05 34 12，是一个 3 字节命令；
  2. `X`，表示扩展 `extend` 的意思；
  3. 内存地址的指定方法，不仅可以使用常数，还可以使用寄存器。但只有 `BX, BP, SI, DI` 这几个可作此用途，剩余的 4 个不能用来指定内存地址，这是因为 `CPU` 没有处理这种指令的电路；如果想把 `DX` 内存里的内容赋值给 `AL`，则可以：`MOV BX, DX`，`MOV AL, BYTE [BX]`；
- 32 位寄存器
  - `EAX`
  - `ECX`
  - `EDX`
  - `EBX`
  - `ESP`
  - `EBP`
  - `ESI`
  - `EDI`
  1. `E` 还是来源于 `extend`，即表示扩展，由 16 位寄存器扩展而来；
  2. 虽说 `EAX` 是个 32 位寄存器，但其实跟前面一样，它有一部分是与 `AX` 共用的，32 位中的低 16 位就是 `AX`，而高 16 位既没有名字，又没有寄存器编号。也就是说，虽然可以把 `EAX` 作为 2 个 16 位寄存器来用，但只有低 16 位用起来方便。如果想要用高 16 位的话，就需要使用移位命令，把高 16 位移到低 16 位之后才能用；
  3. 在指定内存地址的地方，如果使用 16 位寄存器指定 `[CX]` 或 `[SP]` 之类的就会出错，但使用 32 位寄存器，连 `[ECX], [ESP]` 等都 OK，基本上没有不能使用的寄存器；
  4. 在 32 位和 16 位模式下，均可使用寄存器加/减一个常数的方式来指定内存地址；
  5. 如果与 C 语言联合使用的话，有的寄存器能自由使用，有的则不能。能的是 `EAX, ECX, EDX`，其他寄存器只能使用其值，不能改变其值；
  6. 根据 C 语言的规约，执行 `RET` 语句时，`EAX` 中的值就被看作是函数的返回值；
- 段寄存器 `segment register`，16 位：
  - `ES`: 附加段寄存器，`extra segment`；
  - `CS`: 代码段寄存器，`code segment`；
  - `SS`: 栈段寄存器，`stack segment`；
  - `DS`: 数据段寄存器，`data segment`；
  - `FS`: 没有名称，`segment part 2`；
  - `GS`: 没有名称，`segment part 3`；
- 特殊寄存器：
  - `FLAGS`-16bits, `EFLAGS`-32bits: `FLAGS` 是**存储进位标志和中断标志等标志**的寄存器；进位标志可以通过 `JC` 或 `JNC` 等跳转指令来简单地判断到底是 0 还是 1（进位标志是第 0 位）；对于中断标志，没有类似 `JI` 或 `JNI` 命令，所以只能读入 `EFLAGS`，再检查第 9 为是 0 还是 1（中断标志是第 9 位）；能够用来读写 `EFLAGS` 的，只有 `PUSHFD` 和 `POPFD`指令（`MOV` 指令不可），`push flags double-word` 和 `pop flags double-word`；`PUSHFD POP EAX` 表示首先将 `EFLAGS` 压入栈，再将弹出的值代入 `EAX`，`PUSH EAX POPFD` 正与此相反，表示将 `EAX` 的值赋给 `EFLAGS`；
#### 从开机到系统运行的启动过程
- 启动扇区的加载，见“汇编文件进一步加工”；
---
几个概念：`boot, bootloader, bios, u-boot, grup...`
- `bios`: `basic input output system`，出厂时就组装在电脑主板上的 `ROM` 中，电脑厂家在 `bios` 中预先写入了操作系统开发人员经常会用到的一些程序；
- `bios` 与 `boot` 程序：
  - 严格说 `BIOS ≠ boot` 程序，而是 `boot` 流程的“第一棒”，它只做最底线的硬件初始化 + 把真正的 `boot` 代码搬进内存并跳转：
    | 项目   | BIOS                            | Boot 程序（Bootloader）        |
    | ---- | ------------------------------- | -------------------------- |
    | 存放位置 | 主板 ROM（Flash）                   | 磁盘/SSD/U 盘第 0 扇区或 EFI 分区   |
    | 运行时机 | 上电立刻执行                          | BIOS/UEFI 跳转之后             |
    | 任务   | POST、提供底层 I/O 接口、找启动设备、加载 512 B | 加载操作系统内核或第二阶段 loader       |
    | 大小   | 通常 1–16 MB                      | 传统 MBR 仅 512 B；UEFI 可 MB 级 |
    | 是否常驻 | 常驻内存高端，提供中断服务                   | 通常把控制权交给 OS 后就退出           |
  - `BIOS` 是“**固件引导器**”，负责把“第一道 `bootloader`”搬进 `RAM` 并跳过去；
  - `Boot` 程序是“**软件引导器**”，负责把**操作系统**本身拉进内存并启动；
  - `BIOS` 可以看成 `boot` 流程的“第 0 阶段”，但不是通常所说的 `boot` 程序；
  - `CPU` 出厂时 没有 `BIOS`；主板厂/社区把 `BIOS/bootloader` 烧进 外部 `SPI-Flash`；只要主板允许刷写，就能掌控从 `Reset Vector` 起的每一行代码。

- `bios` 流程：
  - BIOS 的实现分三层：1.上电第一条指令（`reset vector`）；2.早期硬件初始化（芯片级）；3.建立软件环境并加载 bootloader；
  - 全程用纯 16 位实模式汇编 + 少量 C 写成，固化在主板 `SPI-Flash` 里，上电后由 `CPU` 直接取指执行。如下按时间线给出主流 `UEFI BIOS（Intel/AMI/Insyde）`与传统 `Legacy BIOS` 都遵循的典型流程：
    1. `CPU` 一上电（`Reset Vector`）：所有 `x86 CPU` 复位后进入 `0xFFFFFFF0`（-16 MB 处），该地址映射到主板 `SPI-Flash` 顶部，第一条指令通常是：`jmp far 0xF000:0xE05B   ; 跳到 Flash 内部的 16 位入口`；此时内存控制器还没初始化，只能使用 `CPU` 内部 `Cache-as-RAM（CAR）`作为临时栈。
    2. 极早期硬件初始化（`PEI` 阶段，`UEFI` 术语）：1.设置 `CAR（Cache-as-RAM）`，得到几 `KB` 可用栈；2.配置时钟、电源管理（PCH 寄存器）；3.训练内存 `PHY` → 把 `DIMM` 参数写到 `SPD`，执行 `MRC（Memory Reference Code）`；4.内存可用后，`CAR` 关闭，栈搬到 `DRAM` 低端（`0x7000` 左右）；
    3. 建立软件环境（`DXE / Legacy POST`）：
        | Legacy 模式                           | UEFI 模式                   |
        | ----------------------------------- | ------------------------- |
        | 扫描 ISA/PCI 设备 → 填写 **IRQ 路由表**      | 加载 DXE Driver（\*.efi）     |
        | 初始化键盘、RTC、IDE/SATA 控制器              | 枚举 PCI/ACPI/USB/Graphics  |
        | 填写 **BIOS Data Area (BDA)** 0x00400 | 构建 **ACPI table**、SMBIOS  |
        | 提供 **INT 13h/10h/16h** 等实模式接口       | 提供 **UEFI Boot Services** |
    4. 寻找启动设备并加载 `Bootloader`：1.按 `BBS`（`BIOS Boot Specification`）顺序扫描：`SATA-0 → USB → NVMe → PXv4 → PXv6 …`；2.把选中设备的 `LBA 0` 读进内存 `0x7C00（Legacy）`或 `\EFI\BOOT\BOOTX64.EFI（UEFI）`；3.校验最后两字节是否为 `0x55 0xAA（Legacy）`或 `PE/COFF` 签名（`UEFI`）；4.跳转到 `0x7C00` 或 `StartImage(BootX64.efi)`，把控制权交给 `Bootloader`；
    5. 代码实现手段：前 `4 KB` 几乎全是 16 位汇编（`Intel` 语法）；内存初始化后可用 `C（Xcode/VisualAge/ GCC 16-bit）`；固件载体：`8 MB/16 MB SPI-NOR Flash`，顶部 `4 KB` 为 `Boot Block`，包含 `Reset Vector` 和早期 `PEI` 代码；调试口：`80h IO` 端口 `POST` 码、`LPC/SIO UART、JTAG/DCI`；标准模块：`Intel FSP（Firmware Support Package）、AMI MRC、EDK2/DXE` 驱动框架；

- `bios, grup, boot, bootload, u-boot`：
    ```bash
    上电 ──► 固件 ──► Bootloader ──► 操作系统内核
              │           │
            BIOS/UEFI   grub/u-boot/etc.
    ```
  - `BIOS / UEFI` —— 固件（`firmware`）:
    - 存在位置：主板 `SPI-Flash`；
    - 运行时机：`CPU` 复位后立刻执行；
    - 任务：初始化时钟、内存、PCI、USB、显示 …；提供 `INT 10h/13h/15h（Legacy）`或 `BootServices/RuntimeServices（UEFI）`；按启动顺序找到“下一阶段映像”并加载，然后把控制权交给它。
  - `Boot / Bootload` —— 通称“引导阶段”：
    - 英文里 boot = bootstrap，bootloader = 引导器；中文常混用；
    - 按场景再分两级：第一阶段 `bootloader`（`MBR 512 B、UEFI BOOTX64.EFI、SPL`）；第二阶段 `bootloader`（`grub2、u-boot proper、Windows bootmgr`）；
  - `grub / u-boot / bootmgr` —— 具体的 `bootloader` 实现
    - `grub（GRand Unified Bootloader）`：面向 `PC`，支持 `Legacy BIOS` 与 `UEFI`；功能：文件系统解析、多系统菜单、加载 `Linux / Windows / initrd`；
    - `u-boot（Universal Boot Loader）`：面向嵌入式`/ARM/PowerPC/RISC-V`；常分成 `SPL`（第一级）+ `u-boot proper`（第二级）；能做 `DDR` 初始化、网络 `TFTP`、烧写 `Flash`、启动 `Linux/Android`；
    - `Windows bootmgr`：微软的第二阶段 `loader`，负责加载 `winload.efi` → 内核；
  - `BIOS/UEFI` 把机器从“砖头”带到“内存可用、设备枚举完成”状态 -> 找到磁盘里的 `grub` 或 `u-boot` 或 `bootmgr` -> 这些 `bootloader` 再把 `OS` 内核 搬进内存并跳转 -> 内核开始运行，整个引导结束

- `MCU` 中的 `bootloader`：
    `MCU` 圈里说的 “`bootloader`” 通常指 “片上 `BootROM` 之后、用户应用程序之前” 的那一小段 用户可改写的程序——它确实相当于 `PC` 概念里的 第一阶段 `bootloader`，但实现和叫法跟 `PC` 略有差别：
  - 片上固化 `BootROM`（`mask-ROM`）：由芯片厂写死，上电先跑；负责把用户 `boot` 映像从 `UART/SPI/I²C/USB/Flash` 搬到 `RAM` 或内部 `SRAM`；有的厂商把它叫 `primary bootloader` 或 `ISP loader`；
  - 用户 `bootloader`（就是 `MCU` 开发者口中的 `bootloader`）：放在 `Flash` 起始区域，大小几 KB～几十 KB；任务：初始化时钟、RAM、外设 → 通过 `UART/USB/CAN/OTA` 接收新固件 → 写入应用区 → 跳转到应用；厂商文档里常叫 `secondary bootloader` 或 `user bootloader`；对应到 `PC` 的术语，它正是 第一阶段 `bootloader`（`MBR / SPL` 的角色）；
  - 用户应用程序：放在 `bootloader` 之后；
  - `MCU` 开发者说的 “`bootloader`” ≈ `PC` 的 `first-stage bootloader`，只是 `MCU` 里没有 `BIOS/UEFI`，也没有 `512 B` 限制，常常把“第一阶段”和“第二阶段”合并成一段用户代码罢了。

## 03 Day
### 制作真正的 `IPL`
```asm
MOV AX, 0x082
MOV ES, AX
MOV CH, 0       ; 柱面 0
MOV DH, 0       ; 磁头 0
MOV CL, 2       ; 扇区 2

MOV AH, 0x02    ; AH=0x12 : 读盘
MOV AL, 1       ; 1 个扇区
MOV BX, 0       
MOV DL, 0x00    ; A 驱动器
INT 0x13        ; 调用磁盘 BIOS
JC error
```
- `BIOS 0x13` 号函数，读写磁盘：
  - `AH`: `0x02-read`, `0x03-write`, `0x04`-校验, `0x0c`-寻道；
  - `AL`: 处理对象的扇区数量，只能同时处理连续的扇区；
  - `CH`: 柱面号 & `0xff`；
  - `CL`: 扇区号（0-5 位） | （柱面号 & 0x300） >> 2；
  - `DH`: 磁头号；
  - `DL`: 驱动器号；
  - `ES:BX`: 缓冲地址（校验及寻道时不使用）；
  - 返回值：`FLACS.CF == 0`，表示没有错误，`AH == 0`；`FLACS.CF == 1`，表示有错误，错误号码存入 `AH` 内（与重置 `reset` 功能一样）；其中 `FLACS.CF` 即进位标志；
- 一张软盘有 80 个柱面（`cylinder`），2 个磁头，18 个扇区（`sector`），且一个扇区有 512 个字节，于是一张软盘的容量是 `80 * 2 * 18 * 512 = 1474560 Byte = 1440 KB`；
- 含有 `IPL` 的启动区，位于 `C0-H0-S1`（柱面 0，磁头 0，扇区 1，扇区从 1 开始），下一个扇区是 `C0-H0-S2`，这次想要装载的就是这个扇区；
- `ES:BX`，缓冲区地址，表明要把从软盘上读出的数据装载到内存的哪个位置；表示方式：段:偏移量，即 `ES << 4 + BX`，于是可以通过两个 16 位寄存器表示出 `1MB` 的寻址空间；上述代码表示将装载的扇区加载到 `0x8200 ~ 0x83ff, 512 Byte` 的内存地址上；另外，`0x8000 ~ 0x81ff` 这 512 字节是留给启动区的；`0x7c00 ~ 0x7dff` 用于启动区，`0x7e00` 以后直到 `0x9fbff` 为止的区域都没有特殊的用途，操作系统可以随便使用；
- **不管要指定内存的什么地址，都必须同时指定段寄存器**，这是规定。对于上述 `ES:BX`，可以写成 `MOV AL, [ES:BX]`，表示将 `ES:BX` 的值作为内存地址，读取该地址中的内容到 `AL` 中；**一般如果省略段寄存器，则默认使用 `DS`**，即 `MOV AL, [SI]` 被解析为 `MOV AL, [DS:SI]`，因此先给 `DS` 赋值为 0；
```asm
; 读磁盘
        MOV   AX, 0x0820
        MOV   ES, AX
        MOV   CH, 0
        MOV   DH, 0
        MOV   CL, 2

        MOV   SI, 0       ; 记录失败次数的寄存器
retry:
        MOV   AH, 0x02
        MOV   AL, 1
        MOV   BX, 0
        MOV   DL, 0x00
        INT   0x13
        JNC   fin         ; 没有出错的话就跳转到 fin
        ADD   SI, 1
        CMP   SI, 5
        JAE   error
        MOV   AH, 0x00
        MOV   DL, 0x00    ; A 驱动器
        INT   0x13        ; 重置驱动器
        JMP   retry
```
- 软盘数据读取时并不一定能够一次就成功，上述实现当数据读取失败时继续读取，直到读取次数达到 5 次仍失败时跳转到错误处理；
- `AH=0x00, DL=0x00, INT 0x13` 表示系统复位；从而实现复位软盘状态，再读一次；
```asm
; 读磁盘
        MOV   AX, 0x0820
        MOV   ES, AX
        MOV   CH, 0       ; 柱面 0
        MOV   DH, 0       ; 磁头 0
        MOV   CL, 2       ; 扇区 2
readloop:
        MOV   SI, 0
retry:
        MOV   AH, 0x02
        MOV   AL, 1
        MOV   BX, 0
        MOV   DL, 0x00
        INT   0x13
        JNC   next        ; 没有出错的话就跳转到 next
        ADD   SI, 1
        CMP   SI, 5
        JAE   error
        MOV   AH, 0x00
        MOV   DL, 0x00
        INT   0x13
        JMP   retry
next:
        MOV   AX, ES
        ADD   AX, 0x0020
        MOV   ES, AX      ; 因为没有 ADD ES, 0x0020 指令
        ADD   CL, 1       ; 读取下一个扇区
        CMP   CL, 18
        JBE   readloop    ; 读到 18 （含第 18） 个扇区
```
- 上述实现读取第 2-18 个扇区的数据到以 0x0820 开始的内存地址上，具体区间为 `0x0820 ~ 0x29ff`（书中是 `0x8200 ~ 0xa3ff`），17 个扇区，数据量为 `17 x 512 = 8704 Byte`；
- 其中 `ADD AX, 0x0020` 中的 `0x0020 = 512 / 16`，也可以直接写成 `ADD AX, 512/16` 或者往 `BX` 加上 512，但需要注意后续循环中给 `BX` 赋值为 0 了；
- 上述读取 17 个扇区的数据，可以直接写成 `AL = 17`。但是在磁盘 `BIOS` 读盘函数说明中指出：“指定处理的扇区数，范围在 `0x01 ~ 0xff`（指定 `0x02` 以上数值时，要特别注意能够连续处理多个扇区的条件。如果是 `FD` 的话，似乎不能跨越多个磁道，也不能超过 64KB 的界限）”；
```asm
CYLS	  EQU		10

; 读磁盘
        MOV   AX, 0x0820
        MOV   ES, AX
        MOV   CH, 0       ; 柱面 0
        MOV   DH, 0       ; 磁头 0
        MOV   CL, 2       ; 扇区 2
readloop:
        MOV   SI, 0
retry:
        MOV   AH, 0x02
        MOV   AL, 1
        MOV   BX, 0
        MOV   DL, 0x00
        INT   0x13
        JNC   next
        ADD   SI, 1
        CMP   SI, 5
        JAE   error
        MOV   AH, 0x00
        MOV   DL, 0x00
        INT   0x13
        JMP   retry
next:
        MOV   AX, ES
        ADD   AX, 0x0020
        MOV   ES, AX      
        ADD   CL, 1       
        CMP   CL, 18
        JBE   readloop
        MOV   CL, 1
        ADD   DH, 1
        CMP   DH, 2
        JB    readloop
        MOV   DH, 0
        ADD   CH, 1
        CMP   CH, CYLS
        JB    readloop
```
- 上述实现连续读取 10 个柱面的内容；
- 读取的内容：`10 x 2 x 18 x 512 = 184 320 Byte = 180 KB`，装载区间：`0x08200 ~ 0x34fff`；

### 着手开发操作系统
- 最简单的操作系统程序：
  ```asm
  fin:
          HLT
          JMP fin
  ```
  - 大概思路就是上述程序被写到 `.sys` 文件中，然后再把该文件保存到磁盘映像里，最后从启动区执行这部分内容即可。而真正的 `OS` 大致也是这个思路；
  - 操作流程：
    - `make img` 制作映像文件；使用到的工具是 `nask.exe` 和 `edimg.exe`；
  - 通过二进制文件查看器查看 `.img` 文件，可知：`.sys` 文件在空软盘保存文件时：1. 文件名会写在 `0x002600` 以后的地方；文件的内容会写在 `0x004200` 以后的地方；
- 从启动区执行操作系统
  - 需要执行磁盘映像上位于 `0x004200` 号地址的程序。现在的程序是从启动区开始，把磁盘上的内容装载到内存 `0x8000` 号地址，所以磁盘 `0x4200` 处的内容就应该位于内存 `0x8000 + 0x4200 = 0xc200` 号地址；
  - 在 `haribote.nas` 中通过 `ORG` 指令指明程序将要被装载到的内存位置，并且在启动区文件 `ipl.nas` 添加相应的跳转指令，以实现完成必要的初始化后跳转到 `haribote.nas` 中的程序位置；
  - 为确认 `haribote.nas` 中程序的执行情况，改为：
  ```asm
  ; haribote-os
  ; TAB=4

          ORG   0xc200

          MOV   AL, 0x13
          MOV   AH, 0x00
          INT   0x10
  fin:
          HLT
          JMP   fin
  ```
  - `ORG 0xc200` 指明程序将要加载的位置；
  - 设置显卡模式：
    - `AH=0x00`；
    - `AL=模式`：`0x03`，表示 16 色字符模式，80 x 25；`0x12`，表示 `VGA` 图形模式，640 x 480 x 4 位彩色模式，独特的 4 面存储模式；`0x13`，表示 `VGA` 图形模式，320 x 200 x 8 位彩色模式，调色板模式；`0x6a`，扩展 `VGA` 图形模式，800 x 600 x 4 位彩色模式，独特的 4 面存储模式（有的显卡不支持这个模式）；
    - 返回值：无
  - 另外，想要把磁盘装载内容的结束地址告诉给 `haribote.sys`，所以在 `JMP 0xc200` 之前，加入了一行命令，将 `CYLS` 的值写到内存地址 `0x0ff0` 中；
- 32 位模式前期准备
  - 如果以 16 位模式启动的话，用 `AX` 和 `CX` 等 16 位寄存器会非常方便，但反过来，向 `EAX` 和 `ECX` 等 32 位的寄存器，使用起来就很麻烦；
  - 16 位模式和 32 位模式中，机器语言的命令代码不一样，同样的机器语言，解释方式也不一样，所以 16 位模式的机器语言在 32 位模式下不能运行，反之亦然；
  - 32 位模式下可以使用的内存容量远远大于 `1MB`，另外 `CPU` 的自我保护功能在 16 位下不能用，但在 32 位下能用；
  - 32 位模式下不能使用 `BIOS`，因为 `BIOS` 是由 16 位机器语言写的；所以如果需要用 `BIOS` 来做的事情，尽量放在开头先做；
  - 更改 `haribote.nas` 以记录相关 `BOOT_INFO`：
  ```asm
  ; haribote-os
  ; TAB=4
  ; 有关 BOOT_INFO

  CYLS    EQU   0x0ff0          ; 设定启动区
  LEDS    EQU   0x0ff1
  VMODE   EQU   0x0ff2          ; 关于颜色数目的信息，颜色的位数
  SCRNX   EQU   0x0ff4          ; 分辨率的 x
  SCRNY   EQU   0x0ff6          ; 分辨率的 y
  VRAM    EQU   0x0ff8          ; 图像缓冲区的开始地址

          ORG   0xc200
          MOV   AL, 0x13
          MOV   AH, 0x00
          INT   0x10
          MOV   BYTE  [VMODE], 8  ; 记录画面模式
          MOV   WORD  [SCRNX], 320
          MOV   WORD  [SCRNY], 200
          MOV   DWORD [VRAM], 0x000a0000
  
  ; 用 BIOS 取得键盘上各种 LED 指示灯状态
          MOV   AH, 0x02
          INT   0x16            ;  keyboard BIOS
          MOV   [LEDS], AL
  
  fin:
          HLT
          JMP   fin
  ```
  - `VRAM` 指的是显卡内存，可以像一般内存一样用来存储数据，同时它的各个地址都对应着画面上的像素，可以利用这一机制在画面上绘制出五彩缤纷的图案；
  - `VRAM` 分布在内存分布图上好几个不同的地方，因为不同的画面模式的像素数也不一样，不同画面模式可以使用的内存也不一样；
  - 在 `INT 0x10` 中说明，在 `AL, 0x12   AH, 0x00` 模式下 `VRAM` 是 `0xa0000 ~ 0xaffff` 的 64KB；
  - 另外，还把画面的像素数、颜色数，以及从 `BIOS` 取得的键盘信息都保存了起来。保存的位置是在内存 `0x0ff0` 附近，从内存分布图来看，这一块并没有被使用；

### 开始导入 C 语言
- 切换到 32 位模式，然后使用 C 语言写的程序。将 `haribote.sys` 转换为 `asmhead.nas`，以实现调用 C 语言写的程序；
  - `todo`
- 从 `.c` 文件到可执行文件（机器语言）
  - 使用 `cc1.exe` 将 `.c` 文件转换成 `.gas` 文件；
  - 使用 `gas2nask.exe` 从 `.gas` 生成 `.nas` 文件；
  - 使用 `nask.exe` 从 `.nas` 生成 `obj` 文件；
  - 使用 `obj2bim.exe` 从 `.obj` 生成 `.bim` 文件；
  - 使用 `bim2hrb.exe` 从 `.bim` 生成 `.hrb` 文件；
  - 这样就做成了机器语言，再使用 `copy` 指令将 `asmhead.bin` 与上述 `.hrb` 单纯结合起来，就成了 `haribote.sys`；
  其中：
  1. `cc1.exe` 将 `.c` 文件编译成汇编语言（`cc1.exe` 是作者从 `gcc` 编译器改造而来，而 `gcc` 又是以 **`gas` 汇编语言**为基础，输出的是 `gas` 用的源程序，它不能直接翻译成 `nask`）；
  2. `gas2nask.exe` 将 `gas` 变换成 `nask` 能翻译的语法；
  3. `nask.exe` 将目标转换成机器语言，制作成 `.obj` 文件；
  4. `obj2bim` 将目标文件进行链接，生成 `binary image, bim` 二进制映像文件，以制作完整的机器语言文件；
  5. `bim2hrb.exe` 将目标转换成实际能够使用的文件。完整的机器语言文件为了能实际使用，还需要针对每个不同的操作系统要求进行必要的加工，比如说加上识别用的文件头，或者压缩等。
- 关于 `bootpack.c` 文件的一些说明：
  1. 在 `bootpack.c` 中的函数 `void HariMain(void)` 不能随便更换签名，因为在 `startup.c` 中的入口函数调用了 `HariMain()`；`todo`: `startup.c` 又是从哪里指定的？或者说哪里又指定了入口函数是 `startup.c` 中的 `HariStartup`？相关工具的源码 `omake\tolsrc`
  2. 在 `bootpack.c` 中使用了 `void io_hlt(void)` 函数，但并没有指定 `extern` 关键字。**是因为函数声明默认就是 `extern`，且与实现语言是 C、C++ 还是汇编毫无关系**；**C 编译器会给 `.c` 文件中的函数名添加修饰符，如 `_`**；
- `naskfunc.nas` 来实现 `HLT` 的调用，减少耗电
  - C 语言中不能使用 `HLT` 指令；
  - 也没有相当于 `DB` 的命令，所以不能用 `DB` 来放一句 `HLT` 语句；
  - 用汇编写的函数，之后还要与 `bootpack.obj` 链接，所以也需要编译成目标文件。因此将输出格式设定为 `WCOFF` 模式。另外，还要设定成 32 位机器语言模式；
  - 在 `nask` 目标文件的模式下，必须设定文件名信息，然后再写明下面程序的函数名。注意要在函数名的前面加上 `_`，否则就不能很好地与 C 语言函数链接；**需要链接的函数名，都需要用 `GLOBAL` 指令声明**；

## 04 Day
### 结合 C 语言对界面进行操作
- 用 C 语言实现内存写入
  - 如果想要画点东西，只要往 `VRAM` 里写点什么即可；
  - 修改 `naskfunc.nas` 增加 C 语言能够调用的写入内存函数：
    ```asm
    _write_mem8:                          ; void write_mem8(int addr, int data);
                MOV ECX, [ESP + 4]        ; [ESP + 4] 中存放的是地址，将其读入 ECX
                MOV AL, [ESP + 8]         ; [ESP + 8] 中存放的是数据，将其读入 AL
                MOV [ECX], AL
                RET
    ```
    - 上述代码实现将 `data` 保存到 `addr` 指定的内存地址上；其中，函数参数指定的数字就存放在内存里，分别是：第一个数字的存放地址：`[ESP + 4]`，第二个数字的存放地址：`[ESP + 8]`，第三个数字的存放地址：`[ESP + 12]`，......；
    - 因为 CPU 已经是 32 位模式，所以积极使用 32 位寄存器；16 位寄存器也不是不能用，但是如果用了的话，不只是机器语言的字节数会增加，而且执行速度也会变慢；
    ```asm
    ; naskfunc.nas
    [INSTRSET "i486p"]
    ```
    - 上述代码指明当前程序是给 `486`CPU 使用的，于是 `nask`（汇编语言编译器）将 `EAX` 等解释为寄存器名；如果什么都不指定，则将其作为标签（`Label`）或常数处理；
    - 上述不仅仅为能在 `486` 中执行的机器语言，如果只使用 16 位寄存器，也能成为 16 位 CPU 中亦可执行的机器语言；其次，它还能在 `386` 下运行；
  - C 语言中的处理：给显卡内存区域写入数据 `15`，第 15 种颜色正好是白色；
- 条纹图案
  - 更改 `bootpack.c`：
    ```c
    for(int i = 0xa0000; i <= 0xaffff; i++){
      write_mem8(i, i & 0x0f);
    }
    ```
    - 上述代码实现一个循环，将数据 `0x00~0x0f` 依次写入内存。这样实现了每隔 16 个像素，色号就反复一次；
- 使用指针
  - 使用 `*i = i & 0x0f` 替代 `write_mem8(i, i & 0x0f)` 会有问题：`invalid type argument of unary *`（类型错误， 是因为 `i` 的类型是 `int`，不是指针类型？）；
  - `*i = i & 0x0f` 的编译结果相当于 `MOV [i], (i & 0x0f)`，它并没有指定数据的大小到底是 `BYTE`，还是 `WORD`，还是 `DWORD`；目的是要完成：`MOV BYTE [i], (i & 0x0f)`；
    修改方式：
    1. 增加 `char *` 变量：`char *p; ... p = (char *)i; *p = i & 0x0f;`
    2. 强制转换 `i` 为 `char *`: `*((char *)i) = i & 0x0f;`
  - 也可以这样写：
    ```c
    p = (char*)0xa0000;
    for(i = 0; i < 0xffff; i++){
      *(p + i) = i & 0x0f;
    }
    ```
  - 或者：
    ```c
    p = (char*)0xa0000;
    for(i = 0; i < 0xffff; i++){
      p[i] = i & 0x0f;
    }
    ```
- 色号设定
  - 这次使用的是 `320 x 200` 的 8 位颜色模式，色号使用 8 （二进制）位数，也就是只能使用 `0~255` 的数；这个 8 位彩色模式，是由程序员随意指定 `0~255` 的数字所对应的颜色，比如说 25 号颜色对应 `#ffffff`，这种方式叫做调色板（`palette`）；
  - 要想描绘一个操作系统模样的画面，只要有以下几种颜色就够了：
    ```css
    #000000: 黑     #00ffff: 浅蓝   #000084: 暗蓝
    #ff0000: 亮红   #ffffff: 白     #840084: 暗紫
    #00ff00: 亮绿   #c6c6c6: 亮灰   #008484: 浅暗蓝
    #ffff00: 亮黄   #840000: 暗红   #848484: 暗灰
    #0000ff: 亮蓝   #008400: 暗绿
    #ff00ff: 亮紫   #848400: 暗黄
    ```
  - 将颜色初始化和相关函数添加到 `bootpack.c` 和 `naskfunc.nas` 中，其中：
    - C 语言中的 `static char` 语句只能用于数据，相当于汇编中的 `DB` 指令；
      ```c
      char a[3];
      ```
      `a` 为常数，以汇编语言来讲就是标识符，标识符的值相当于地址。并且还准备了 `RESB 3`：
      ```asm
      a:
          RESB 3
      ```
      在 `nask` 中 `RESB` 的内容能够保证是 0；但 C 语言中不能，保证所以里面说不定含有某些垃圾数据；
      同时，为了避免每次函数调用时内存消耗，在汇编语言中用 `DB` 指令代替 `RESB` 指令，对应到 C 语言中则是给变量定义时加上 `static` 关键字；
      ```asm
      table_rgb:
        DB 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, ...
      ```
    - 将 `0xff` 即 255 对应的变量定义为 `unsigned`，从而避免将 `0xff` 误解为负数，即 -1；
    - `0x03c8`，`0x03c9` 之类为设备号码；调色板的访问步骤：
      1. 首先在一连串的访问中屏蔽中断（比如 `CLI`）；
      2. 将想要设定的调色板号码写入 `0x03c8`，紧接着按 `R, G, B` 的顺序写入 `0x03c9`。如果还想继续设定下一个调色板，则省略调色板号码，再按照 `R, G, B` 的顺序写入 `0x03c9`；
      3. 如果想要读出当前调色板的状态，首先将调色板的号码写入 `0x03c7`，再从 `0x03c9` 读取 3 次。读出的顺序就是 `R, G, B`。如果想要继续读出下一个调色板，同样也是省略调色板号码的设定，按 `R, G, B` 的顺序读出；
      4. 如果最初执行了 `CLI`，那么最后要执行 `STI`；
    - `io_out8(0x03c9, rgb[0] / 4);` 除以 4 是因为 `DAC` 的规格是 6 位；另外，在 `HariMain` 中使用了 `0~15` 号调色板对应的元素，因此在调色板设置时也设置 `0~15` 号对应的颜色；
    - 通过 `io_load_eflags` 函数记录中断许可标志的值；
    - 通过 `io_cli` 函数将中断许可标志置为 0，禁止中断；
    - 通过 `io_store_eflags` 函数复原给定参数的中断许可标志；
- 绘制矩形
  - 使用 `define` 来定义颜色常量；
  - 画面上有 `320 x 200` 个像素，假设左上角的坐标是 `(0, 0)`，右下角的坐标是 `(319, 199)`，则 `(x, y)` 对应的 `VRAM` 地址计算方式为 `0xa0000 + x + y * 320`（`x` 是水平方向，`y` 是垂直方向）；

### 扩展知识
- 函数参数在栈中的保存顺序与函数调用约定相关，具体可见《程序员的自我修养》阅读笔记；
- `Intel` CPU 发展史：`8086 -> 80186 -> 286 -> 386 -> 486 -> Pentium -> PentiumPro -> PentiumII -> PentiumIII -> Pentium4 -> ...`，到 `286` 为止 CPU 是 16 位，而 `386` 以后 CPU 为 32 位；
- `char` 占 1 字节内存，`short` 占 2 字节内存，`int` 占 4 字节内存；指针（不管什么类型指针）都占 4 字节内存（32 位 CPU）；在汇编语言中，地址也像 `ECX` 一样，用 4 字节的寄存器来指定，所以也是 4 字节；
- 声明指针时使用类似于 `char *p` 而不使用 `char* p` 的形式，避免需要定义两个指针却使用 `char* p, q;` 而导致的错误；
- `p = (char *)i; *p = i & 0x0f;` 转换为汇编：`MOV ECX, i`，`MOV BYTE [ECX], i & 0x0f`；
- `p[i]` 与 `*(p + i)` 等价；由于加法运算可以交换顺序，所以将 `*(p + i)` 写成 `*(i + p)` 也是可以的。同理，将 `p[i]` 写成 `i[p]` 也是可以的；本质上讲，`p[i]` 是一种省略写法，与数组没有关系，**表示相对于 `p` 表示地址的偏移量**；
- `char` 型变量有 3 种模式，`signed`，`unsigned` 以及未指定型，其中未指定型是没有特别指定时，可由编译器决定是 `unsigned` 还是 `signed`；

## 05 Day
### 从 bootinfo 中获取信息
- 单独保存每个 bootinfo 信息
  ```c
  // Harimain.c
  void Harimain(void){
    char *vram;
    int *binfo_vram;
    int xsize, ysize,
    short *binfo_scrnx, *binfo_scrny;

    init_palette();
    binfo_scrnx = (short*) 0x0ff4;    // 对应到 asmhead.nas 中指定的地址
    binfo_srcny = (short*) 0x0ff6;
    binfo_vram  = (int*)   0x0ff8;
    xsize       = *binfo_scrnx;
    ysize       = *binfo_scrny;
    vram        = (char*) binfo_vram;
  }
  ```
- 通过结构体保存 bootinfo 信息
  ```c
  /* BootInfo 占 8 字节 */
  struct BootInfo{
    char cyls, leds, vmode, reserve;
    short xsize, ysize;
    char *vram;
  }

  /* 通过 '.' 来访问结构体成员 */
  void HariMain(void){
    char *vram;
    int xsize, ysize;
    struct BootInfo *binfo;

    init_palette();
    binfo = (struct BootInfo*) 0x0ff0;
    xsize = (*binfo).xsize;
    ysize = (*binfo).ysize;
    vram  = (*binfo).vram;
  }

  /* 通过 '->' 来访问结构体成员 */
  void HariMain(void){
    struct BootInfo *binfo = (struct BootInfo*) 0x0ff0;
    init_palette();
    init_screen(binfo->vram, binfo->srcnx, binfo->srcny);
  }
  ```
  **其中，指针的偏移单位与指针所指实际数据类型大小相关**。如 `struct BootInfo *p; p++;` 导致指针 `p` 在实际内存中偏移 8 个地址；

### 显示字符
- 自定义字符数据
  - 之前显示字符时，主要靠调用 `BIOS` 函数（16 位模式，如 `helloReadable.nas` 中显示 `hello, world`），但现在是 32 位模式，不能再依赖 `BIOS` 了（可以向显卡内存中写入数据）。
  - 字符可以用 `8 x 16` 的长方形像素点阵来表示，于是一个字符占 16 个字节；
  - 定义一个字符 `A` 的数据并显示：
    ```c
    static char font_A[16] = {
      0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24, 
      0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
    };

    /*
    void putfont8(char *vram, int xsize, int x, int y, char c, char *font){
      int i;
      char d;
      for(i = 0; i < 16; ++i){
        d = font[i];
        if((d & 0x80) != 0) (vram[(y + i) * xsize + x + 0] = c);
        if((d & 0x40) != 0) (vram[(y + i) * xsize + x + 1] = c);
        if((d & 0x20) != 0) (vram[(y + i) * xsize + x + 2] = c);
        if((d & 0x10) != 0) (vram[(y + i) * xsize + x + 3] = c);
        if((d & 0x08) != 0) (vram[(y + i) * xsize + x + 4] = c);
        if((d & 0x04) != 0) (vram[(y + i) * xsize + x + 5] = c);
        if((d & 0x02) != 0) (vram[(y + i) * xsize + x + 6] = c);
        if((d & 0x01) != 0) (vram[(y + i) * xsize + x + 7] = c);
      }
      return;
    }
    */

    void putfont8(char *vram, int xsize, int x, int y, char c, char *font){
      int i;
      char *p, d;
      for(i = 0; i < 16; ++i){
        p = vram + (y + i) * xsize + x;
        d = font[i];
        if((d & 0x80) != 0) (p[0] = c);
        if((d & 0x40) != 0) (p[1] = c);
        if((d & 0x20) != 0) (p[2] = c);
        if((d & 0x10) != 0) (p[3] = c);
        if((d & 0x08) != 0) (p[4] = c);
        if((d & 0x04) != 0) (p[5] = c);
        if((d & 0x02) != 0) (p[6] = c);
        if((d & 0x01) != 0) (p[7] = c);
      }
      return;
    }
    ```
- 增加字体
  - 新增字体文件 `hankaku.txt`，通过 `makefont.exe` 将其转换成 `hankaku.bin` 二进制文件（`16 x 256 = 4096 Byte`），之后再由 `bin2obj.exe` 对其添加链接所需的接口信息以形成目标文件（`.obj`），最后完成与 `bootpack.obj` 的链接；转换成 `.obj` 文件相当于：
    ```asm
    _hankaku:
        DB 各种数据（共 4096 字节）
    ```
    在 C 语言中使用：
    ```c
    extern char hankaku[4096];
    ```
    另外，字体数据按照 `ASCII` 字符编码，因此 `A` 的数据放在 `hankaku + 16 * 41` 的位置，`A` 的 `ASCII` 码为 41，还可以直接写字符 `A`。在 `Makefile` 文件对应的更改：
    ```Makefile
    MAKEFONT = $(TOOLPATH)/makefont.exe
    BIN2OBJ	 = $(TOOLPATH)/bin2obj.exe
    
    hankaku.bin : hankaku.txt Makefile
      $(MAKEFONT) hankaku.txt hankaku.bin
    
    hankaku.obj : hankaku.bin Makefile
      $(BIN2OBJ)  hankaku.bin hankaku.obj _hankaku

    bootpack.bin : bootpack.obj naskfunc.obj hankaku.obj Makefile
      $(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
        bootpack.obj naskfunc.obj hankaku.obj
    ```
- 显示变量
  - 在做出调试器之前，只能通过显示变量值来查看确认问题的地方；
  - 可以使用 `sprintf` 来显示，虽然自制操作系统中不能随便使用 `printf` 函数，但 **`sprintf` 可以使用，因为它不是按指定格式输出，而是将输出内容作为字符串写在内存中**；`sprintf` 为本次使用的是名为 `GO` 的 C 语言编译器附带的函数，能够不使用操作系统的任何功能；`printf`（输出字符串的方法）不可避免地要使用操作系统的功能，而 `sprintf` 不同，它只对内存进行操作，所以可以应用于所有操作系统；
  - 使用 `sprintf` 函数需包含 `<stdio.h>` 头文件；
  - 封装成函数 `putfont8_asc` 以显示 `ASCII` 字符串；**所谓字符串是按顺序排列在内存里，末尾再加上 `0x00` 而组成的字符编码**；
- 显示鼠标指针
  - 定义鼠标图标数据
    ```c
    void init_mouse_cursor8(char *mouse, char bc){
      static char cursor[16][16] = {
        "**************..",
        "*00000000000*...",
        "*0000000000*....",
        "*000000000*.....",
        "*00000000*......",
        "*0000000*.......",
        "*0000000*.......",
        "*00000000*......",
        "*0000**000*.....",
        "*000*..*000*....",
        "*00*....*000*...",
        "*0*......*000*..",
        "**........*000*.",
        "*..........*000*",
        "............*00*",
        ".............***",
      };
      int x, y;

      for(y = 0; y < 16; ++y){
        for(x = 0; x < 16; ++x){
          if(cursor[y][x] == '*'){
            mouse[y * 16 + x] = COL8_000000;
          }
          if(cursor[y][x] == '0'){
            mouse[y * 16 + x] = COL8_FFFFFF;
          }
          if(cursor[y][x] == '.'){
            mouse[y * 16 + x] = bc;
          }
        }
      }

      return;
    }
    ```
  - 显示图标
    ```c
    void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize){
      int x, y;
      for(y = 0; y < pysize; ++y){
        for(x = 0; x < pxsize; ++x){
          vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
        }
      }
      return;
    }
    ```

## 第一周小结