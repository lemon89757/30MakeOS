# Note for 30MakeOs
## 01 Day
### 二进制文件 `helloos.img`
- `helloos.img` 文件的一种简单生成方式：
  ```bash
  dd if=/dev/zero of=hello.img count=1474560 bs=1
  ```
  通过上述命令生成全为 0 的，大小为 1474560 `byte` 的二进制文件，再根据作者提示在二进制文件相应位置作更改即可；

- `helloos.img` 运行流程：
  ```cmd
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
- `asm.bat`
  ```cmd
  ..\z_tools\nask.exe helloos.nas helloos.img
  ```
  其中`nask` 是作者根据 `NASM` 开发的汇编语言编译器；

### 扩展知识
- `TAB-4`： 表示 `TAB` 键的宽度为 4；
- `FAT12` 格式： `FAT12 Format` 用 `Windows` 或 `MS-DOS` 格式化出来的软盘就是这种格式；（文件系统内容）
- 启动区：
  - `boot sector`，**软盘的第一个扇区称为启动区**；**计算机首先从最初一个扇区开始读软盘，然后去检查这个扇区最后 2 个字节的内容。如果这最后 2 个字节的内容不是 `0x55 0xAA`，计算机会认为这张软盘上没有所需的启动程序，就会报一个不能启动的错误**；
  - 计算机读写软盘的时候并不是一个字节一个字节地读写，而是以 512 字节为单位进行读写。因此，软盘的 512 字节就称为一个扇区；
  - 一张软盘的空间共有 1440KB，也就是 1474560 字节，即 2880 个扇区；
- `IPL`：
  - `initial program loader`，即启动程序加载器；
  - 启动区只有区区 512 字节，实际操作系统根本装不进去。所以几乎所有的操作系统都是把加载操作系统给本身的程序放在启动区里；因此，有时也将启动区称为 `IPL`；
  - 可以将 `HELLOIPL` 换成别的名字，但必须是 8 个字节长度；
- `boot`，即 `bootstrap`，操作系统自动启动机制；

### 汇编文件加工润色
- 汇编文件占 512 个字节，其中最后两个字节的内容为 `0x55 0xAA`（具体原因见后续启动区内容）；


## 第一周小结