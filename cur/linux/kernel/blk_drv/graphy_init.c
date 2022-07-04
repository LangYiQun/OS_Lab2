#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <a.out.h>

#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <asm/segment.h>

#include <sys/types.h>
#include <asm/io.h>

#define vga_graph_memstart 0xA0000
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320		//宽度
#define vga_height 200		//高度




int sys_init_graphics() 
{
	printk("------------------------------------------\n");
//启动Mode 0x13图形模式
	outb(0x05, 0x3CE);
	outb(0x40, 0x3CF);
	outb(0x06, 0x3CE);
	outb(0x05, 0x3CF);
	outb(0x04, 0x3C4);
	outb(0x08, 0x3C5);

//设置屏幕分辨率320 *200
	outb(0x01, 0x3D4);
	outb(0x4F, 0x3D5);
	outb(0x03, 0x3D4);
	outb(0x82, 0x3D5);

	outb(0x07, 0x3D4);
	outb(0x1F, 0x3D5);
	outb(0x12, 0x3D4);
	outb(0x8F, 0x3D5);
	outb(0x17, 0x3D4);
	outb(0xA3, 0x3D5);

	outb(0x14, 0x3D4);
	outb(0x40, 0x3D5);
	outb(0x13, 0x3D4);
	outb(0x28, 0x3D5);

//建立像素点与显存地址空间的映射
	outb(0x0C, 0x3D4);
	outb(0x0, 0x3D5);
	outb(0x0D, 0x3D4);
	outb(0x0, 0x3D5); 

	int i , j;
	char *p = vga_graph_memstart;
	for (i = 0; i < vga_graph_memsize; i++) {
		*p++ = 3;
	}


	return 0;
}

struct rect {
    long color;
    long x;
    long y;
    long lenth;
    long height;
};

int sys_paintrect(struct rect * rect)
{
    int i, j;
    char * p;
    long color = get_fs_long(&rect->color);
    long x = get_fs_long(&rect->x);
    long y = get_fs_long(&rect->y);
    long dx = get_fs_long(&rect->lenth);
    long dy = get_fs_long(&rect->height);
    for (i = x; i < x+dx; ++i) if (0 <= i && i < vga_width)
        for (j = y; j < y+dy; ++j) if (0 <= j && j < vga_height){
            p = (char *)vga_graph_memstart + vga_width*j + i;
            *p = color;
        }
    return 0;
}