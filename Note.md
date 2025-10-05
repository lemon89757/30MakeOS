# Note for 30MakeOs
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
  - `MOV`: 赋值指令；`eg`: 
    - `MOV AX,0`、`MOV SS,AX`；
    - `MOV AL,[SI]`：读取 `SI` 的值所对应的内存地址并将读取结果赋给 `AL`）；
    - `MOV BYTE [678], 123`：用 `678` 号内存来保存 `123` 这个数值，`BYTE` 表示只占用一个字节内存；`MOV WORD [678], 123`，还是用 `678` 号内存来保存 `123` 这个数值，只不过这次需要使用 `WORD` 即 2 个字节内存，对应为 `678` 和 `679` 号内存。其中 `123d` 即 `0000 0000 0111 1011b` 的 `0000 0000` 保存在 `679` 号内存，剩余的 `0111 1011` 保存在 `678` 号内存；
    - `MOV` 指令有一个规则，就是源数据和目的数据必须相同位数。也就是说，能向 `AL` 里代入的就只有 `BYTE`，这样一来就可以省略 `BYTE`，即可以写成：`MOV AL, [SI]`；
  - `ADD`: 加法指令，`ADD SI, 1` 即表示 `SI += 1`；
  - `CMP`: 比较指令，`CMP AL, 0`，表示如果二者相等或不等，需要做什么（后接条件跳转指令等）；
  - `INT`: 软件中断指令；
  - `HLT`: `halt`（停止），让 `CPU` 停止动作的指令，使 `CPU` 进入待机状态；在循环中可以避免 `CPU` 全力执行 `JMP` 指令，从而达到降低能耗的目的；
  - `EQU`: `equal`，可以用来声明常量，相当于 C 中的 `#define`；
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
- 段寄存器 `segment register`，16 位：
  - `ES`: 附加段寄存器，`extra segment`；
  - `CS`: 代码段寄存器，`code segment`；
  - `SS`: 栈段寄存器，`stack segment`；
  - `DS`: 数据段寄存器，`data segment`；
  - `FS`: 没有名称，`segment part 2`；
  - `GS`: 没有名称，`segment part 3`；
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
- `ES:BX`，缓冲区地址，表明要把从软盘上读出的数据装载到内存的哪个位置；表示方式：段:偏移量，即 `ES << 4 + BX`，于是可以通过两个 16 位寄存器表示出 `1MB` 的寻址空间；上述代码表示将装载的扇区加载到 `0x8200 ~ 0x83ff, 512 Byte` 的内存地址上；另外，`0x8000 ~ 0x81ff` 这 512 字节是留给启动区的；`0x7c00 ~ 0x7dff` 用于启动区，`0x7e00` 以后直到 `0x9fbff` 为止的区域都没有特殊的用途，操作系统可以随便使用；[Memory-Map X86](https://wiki.osdev.org/Memory_Map_(x86))
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

## 第一周小结