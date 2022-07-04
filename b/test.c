#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>


#define __NR_init_graphics	95
#define __NR_timer_create	96
#define __NR_get_message	97


#define MSG_MOUSE_CLICK_L 1
#define MSG_TIMEOUT     2
#define MSG_MOUSE_CLICK_R 3

typedef struct {
    int mid;
    int pid;
} message;

int j = 0;

_syscall0(int,init_graphics);
_syscall1(void,get_message,message*,msg);
_syscall2(int,timer_create,long, a,int, b);



int main()
{
    message *msg;
	msg->mid=0;

    timer_create(1000,1);
    /*init_graphics();*/

    while(1){
        get_message(msg);
        if(msg->mid==MSG_MOUSE_CLICK_L){
            printf("L\n");
        }
        if(msg->mid==MSG_MOUSE_CLICK_R){
            printf("R\n");
        }
        if(msg->mid==MSG_TIMEOUT){
            printf("time_out\n");
            timer_create(1000-(j),1);
			if(j<500) j+=10;
        }
    }
    return 0;
}