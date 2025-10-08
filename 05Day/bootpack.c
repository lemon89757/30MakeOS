void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
void io_store_eflags(int eflags);
int io_load_eflags(void);
void write_mem8(int addr, int data);

void init_palette(void);
void init_screen(char *vram, int x, int y);
void set_paletter(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

#define COL8_000000    0 // 黑
#define COL8_FF0000    1 // 亮红
#define COL8_00FF00    2 // 亮绿
#define COL8_FFFF00    3 // 亮黄
#define COL8_0000FF    4 // 亮蓝
#define COL8_FF00FF    5 // 淡紫
#define COL8_00FFFF    6 // 浅亮蓝
#define COL8_FFFFFF    7 // 白
#define COL8_C6C6C6    8 // 灰
#define COL8_840000    9 // 暗红
#define COL8_008400   10 // 暗绿
#define COL8_848400   11 // 暗黄
#define COL8_000084   12 // 暗青
#define COL8_840084   13 // 暗紫
#define COL8_008484   14 // 浅暗蓝
#define COL8_848484   15 // 暗灰

// void HariMain(void){
//     char *vram;
//     int xsize, ysize;
//     short *binfo_scrnx, *binfo_scrny;
//     int *binfo_vram;

//     init_palette(); // 设置调色板
//     /* 获取 bootinfo 中的信息，其中地址对应到 asmhead.nas 中的偏移 */
//     binfo_scrnx = (short*) 0x0ff4;
//     binfo_scrny = (short*) 0x0ff6;
//     binfo_vram  = (int*)   0x0ff8;
//     xsize = *binfo_scrnx;
//     ysize = *binfo_scrny;
//     vram  = (char*)  *binfo_vram;

//     init_screen(vram, xsize, ysize);

//     for(;;){
//         io_hlt();
//     }
// }

struct BootInfo{
    char cyls, leds, vmode, reserve;
    short scrnx, scrny;
    char *vram;
};

// void HariMain(void){
//     char *vram;
//     int xsize, ysize;
//     struct BootInfo *binfo;

//     init_palette(); // 设置调色板
//     binfo = (struct BootInfo *) 0x0ff0;
//     xsize = (*binfo).scrnx;
//     ysize = (*binfo).scrny;
//     vram  = (*binfo).vram;

//     init_screen(vram, xsize, ysize);

//     for(;;){
//         io_hlt();
//     }
// }

void HariMain(void){
    struct BootInfo *binfo = (struct BootInfo *) 0x0ff0;
    
    init_palette(); // 设置调色板
    init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

    for(;;){
        io_hlt();
    }
}

void init_palette(void){
    static unsigned char table_rgb[16 * 3] = {
        0x00, 0x00, 0x00,   //  0: 黑
        0xff, 0x00, 0x00,   //  1: 亮红
        0x00, 0xff, 0x00,   //  2: 亮绿
        0xff, 0xff, 0x00,   //  3: 亮黄
        0x00, 0x00, 0xff,   //  4: 亮蓝
        0xff, 0x00, 0xff,   //  5: 淡紫
        0x00, 0xff, 0xff,   //  6: 浅亮蓝
        0xff, 0xff, 0xff,   //  7: 白
        0xc6, 0xc6, 0xc6,   //  8: 灰
        0x84, 0x00, 0x00,   //  9: 暗红
        0x00, 0x84, 0x00,   // 10: 暗绿
        0x84, 0x84, 0x00,   // 11: 暗黄
        0x00, 0x00, 0x84,   // 12: 暗青
        0x84, 0x00, 0x84,   // 13: 暗紫
        0x00, 0x84, 0x84,   // 14: 浅暗蓝
        0x84, 0x84, 0x84    // 15: 暗灰
    };
    set_paletter(0, 15, table_rgb);
    return;
}

void set_paletter(int start, int end, unsigned char *rgb){
    int i, eflags;
    eflags = io_load_eflags();   // 记录中断许可标志的值
    io_cli();                    // 将中断许可标志置为 0，禁止中断
    io_out8(0x03c8, start);
    for(i = start; i <= end; i++){
        io_out8(0x03c9, rgb[0] / 4);    // 除以 4 是因为 DAC 的规格是 6 位
        io_out8(0x03c9, rgb[1] / 4);
        io_out8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    io_store_eflags(eflags);     // 复原中断许可标志
    return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
    int x, y;
    for(y = y0; y <= y1; y++){
        for(x = x0; x <= x1; x++){
            vram[y * xsize + x] = c;
        }
    }
    return;
}

void init_screen(char *vram, int xsize, int ysize){
    boxfill8(vram, xsize, COL8_008484,  0,         0,          xsize -  1, ysize - 29);
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
	boxfill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);

	boxfill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
	boxfill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
	boxfill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
	boxfill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
	boxfill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
	boxfill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
	boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
	boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);
}