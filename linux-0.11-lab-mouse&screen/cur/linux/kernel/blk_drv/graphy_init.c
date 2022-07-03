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
#define vga_width 320
#define vga_height 200

int sys_init_graphics() 
{
	printk("%%%%%%%%%%%%%%%%%\n");
	outb(0x05, 0x3CE);
	outb(0x40, 0x3CF);
	outb(0x06, 0x3CE);
	outb(0x05, 0x3CF);
	outb(0x04, 0x3C4);
	outb(0x08, 0x3C5);

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

	outb(0x0C, 0x3D4);
	outb(0x0, 0x3D5);
	outb(0x0D, 0x3D4);
	outb(0x0, 0x3D5); 

	int i;
	char *p = vga_graph_memstart;
	for (i = 0; i < vga_graph_memsize; i++) {
		*p++ = 3;
	}

	return 0;
}