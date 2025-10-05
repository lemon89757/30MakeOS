// 告诉 C 编译器，有一个函数在别的文件里
void io_hlt(void);  

void HariMain(void){
fin:
    // 执行 naskfunc.nas 中的 _io_hlt 函数
    // C 编译器会添加函数修饰符 _
    io_hlt();
    goto fin;
}