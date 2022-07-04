#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>

#define vga_graph_memstart 0xA0000
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320		
#define vga_height 200		

#define __NR_init_graphics	95
#define __NR_timer_create	96
#define __NR_get_message	97
#define __NR_paintrect	    98

#define MSG_MOUSE_CLICK_L 1
#define MSG_TIMEOUT     2
#define MSG_MOUSE_CLICK_R 3

#define MAX_BARRIER 36
#define BARRIER_COLOR 1
#define BARRIER_WIDTH 20
#define BARRIER_HEIGHT 100

#define GAME_OVER_COLOR 2

#define MOV_SPEED 4

struct rect {
    long color;
    long x;
    long y;
    long lenth;
    long height;
};

typedef struct {
    int mid;
    int pid;
} message;

int i,j;
int count = 0;

_syscall0(int,init_graphics);
_syscall1(int,get_message,message*,msg);
_syscall2(int,timer_create,long, a,int, b);
_syscall1(int,paintrect,struct rect *,rect)

struct rect bird1;
struct rect bird2;
struct rect barr_up;
struct rect barr_down;
struct rect barrier[MAX_BARRIER];

int rear = 0;
int head = 0;

int push_barr(struct rect * obj) {
    if (rear != (head + MAX_BARRIER - 1) % MAX_BARRIER) {
        barrier[rear].x = obj->x;
        barrier[rear].y = obj->y;
        barrier[rear].lenth = obj->lenth;
        barrier[rear].height = obj->height;
        rear = (rear + 1) % MAX_BARRIER;
        return 0;
    }
    return -1;
}

int pop_barr(struct rect * obj) {
    if (rear == head)
        return -1;
    if (obj != NULL) {
        obj->x = barrier[head].x;
        obj->y = barrier[head].y;
        obj->lenth = barrier[head].lenth;
        obj->height = barrier[head].height;
    }
    head = (head + 1) % MAX_BARRIER;
    return 0;
}


int paint_barrier(void) {
    int i;
    struct rect rect;
    for (i = head; i != rear; i = (i+1)%MAX_BARRIER) {
        rect.color = BARRIER_COLOR;
        rect.x = barrier[i].x;
        rect.y = barrier[i].y;
        rect.lenth = barrier[i].lenth;
        rect.height = barrier[i].height;
        if (paintrect(&rect) < 0)
            return -1;
    }
    return 0;
}





int paint_GameOver(void) {
    struct rect Whole_screen;
    Whole_screen.color = GAME_OVER_COLOR;
    Whole_screen.x = 0;
    Whole_screen.y = 0;
    Whole_screen.height = vga_height;
    Whole_screen.lenth = vga_width;
    if (paintrect(&Whole_screen) < 0)
        return -1;
    return 0;
}

int main()
{
    message *msg;
	msg->mid=0;

    timer_create(1000,1);

    bird1.color = 9;
    bird1.x = 20;
    bird1.y = 100;
    bird1.lenth = 5;
    bird1.height = 5;

    bird2.color = 8;
    bird2.x = 20;
    bird2.y = 120;
    bird2.lenth = 5;
    bird2.height = 5;

    init_graphics();
    paintrect(&bird1);
    paintrect(&bird2);

    while(1){
        get_message(msg);
        if(msg->mid==MSG_MOUSE_CLICK_L){
            bird1.y-=5;
            paintrect(&bird1);
            paintrect(&bird2);
        }
        if(msg->mid==MSG_MOUSE_CLICK_R){
            bird2.y-=5;
            paintrect(&bird1);
            paintrect(&bird2);
        }
        if(msg->mid==MSG_TIMEOUT){
            init_graphics();
            paintrect(&bird1);
            paintrect(&bird2);

            bird1.y+=5;
            bird2.y+=5;

            if(count++ >= 8)
            {
                barr_up.lenth  = BARRIER_WIDTH;
                barr_up.height = rand()%BARRIER_HEIGHT;
                barr_up.x = vga_width;
                barr_up.y = 0;
                push_barr(&barr_up);

                barr_down.lenth  = BARRIER_WIDTH;
                barr_down.height = rand()%BARRIER_HEIGHT;
                barr_down.x = vga_width;
                barr_down.y = vga_height - barr_down.height;
                push_barr(&barr_down);
               
                count = 0;
            }
            for (i = head; i != rear; i = (i+1)%MAX_BARRIER){
                barrier[i].x -= MOV_SPEED;
                if(barrier[i].x<0) pop_barr(NULL);
                if((bird1.x >= barrier[i].x && bird1.x <= barrier[i].x + barrier[i].lenth
                    && bird1.y >= barrier[i].y && bird1.y <= barrier[i].y + barrier[i].height)||
                    (bird2.x >= barrier[i].x && bird2.x <= barrier[i].x + barrier[i].lenth
                    && bird2.y >= barrier[i].y && bird2.y <= barrier[i].y + barrier[i].height))
                    goto Game_over;
            }
            paint_barrier();

            if(bird1.x <= 0 || bird1.x >= vga_width || bird1.y <= 0 || bird1.y >= vga_height ||
                bird2.x <= 0 || bird2.x >= vga_width || bird2.y <= 0 || bird2.y >= vga_height)
                break;

            timer_create(1000-(j),1);
			if(j<50) j+=10;
        }
    }
    Game_over:
    paint_GameOver();
    return 0;
}