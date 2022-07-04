#include <linux/tty.h>
#include <linux/sched.h>
#include <asm/system.h>
#include <asm/io.h>
#define MSG_MOUSE_CLICK_L 1
#define MSG_MOUSE_CLICK_R 3

//用来记录鼠标输入的第几个字节
static unsigned char mouse_input_count = 0;

//用来记录左键、右键是否按下
static unsigned char mouse_left_down;
static unsigned char mouse_right_down;
//用来记录鼠标是否上下左右移动
static unsigned char mouse_left_move;
static unsigned char mouse_down_move;

//记录鼠标的坐标
static unsigned char mouse_x_position;
static unsigned char mouse_y_position;

#define vga_graph_memstart 0xA0000
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320		//宽度
#define vga_height 200		//高度


void readmouse(int mousecode)
{
    //鼠标命令成功响应
    if(mousecode == 0xFA){
        mouse_input_count = 1;
        return;
    }
    switch(mouse_input_count)
    {
        //处理第一个字节，判断左键、右键是否按下，鼠标上下左右是否移动
        case 1:
            mouse_left_down = (mousecode & 0x1) == 0x1;
            mouse_right_down = (mousecode & 0x2) == 0x2;
            mouse_left_move = (mousecode & 0x10) == 0x10;
            mouse_down_move = (mousecode & 0x20) == 0x20;
            if(mouse_left_down && mousecode == 9){
                //printk("left\n");
                post_message(MSG_MOUSE_CLICK_L);
            }
            if(mouse_right_down && mousecode == 10){
                //printk("right\n");
                post_message(MSG_MOUSE_CLICK_R);
            }
            mouse_input_count++;
            break;
        //处理第二个字节，计算鼠标左右移动
        case 2:
            if(mouse_left_move)
                mouse_x_position += (int)(0xFFFFFF00|mousecode);
            else
                mouse_x_position += (int)(mousecode);
            if(mouse_x_position < 0) mouse_x_position = 0;
            mouse_input_count++;
            //printk("(%d,%d)\n",mouse_x_position,mouse_y_position);
            break;
        //处理第三个字节，计算鼠标上下移动
        case 3:
            if(mouse_down_move)
                mouse_y_position += (int)(0xFFFFFF00|mousecode);
            else
                mouse_y_position += (int)(mousecode);
            if(mouse_y_position < 0) mouse_y_position = 0;
            mouse_input_count=1;
            break;
    }
    // int i , j;
	// char *p = vga_graph_memstart;
	// for (i = 0; i < vga_graph_memsize; i++) {
	// 	*p++ = 3;
	// }
    // for	(i = mouse_x_position - cursor_side; i < mouse_x_position + cursor_side; i++){
	// 	for	(j = mouse_y_position - cursor_side; j < mouse_y_position + cursor_side; j++){
	// 		p = (char*)vga_graph_memstart + j*vga_width + i;
	// 		*p = 12;
	// 	}
	// }
}