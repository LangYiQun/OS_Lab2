#include <linux/sched.h>
#include <asm/segment.h>


int sys_timer_create(long milliseconds, int type)
{
    long jiffies = milliseconds / 10;
    user_timer *timer = (user_timer*)malloc(sizeof(user_timer));
    timer->init_jiffies = jiffies;
    timer->jiffies = jiffies;
    timer->type = type;
    timer->pid = current->pid;
    timer->next = timer_head;//新timer放到头部
    timer_head = timer;
    return 1;
}

void post_message(int type)
{
	if (msg_tail != msg_head - 1) {//消息队列是否满
		message msg;
		msg.mid = type;
		msg.pid = current->pid;
		//cli();
		msg_list[msg_tail] = msg;
		msg_tail = (msg_tail + 1) % MAX_MSG;
		//sti();
	}
}

void sys_get_message(message *msg) {
    message tmp;
	//cli();
	if(msg_tail==msg_head){//消息队列为空
		put_fs_long(0,msg);
		return;
	}
	tmp = msg_list[msg_head];
	msg_list[msg_head].mid = 0;
	msg_head = (msg_head + 1) % MAX_MSG;;
	put_fs_long(tmp.mid,msg);
	return ;
	//sti();

}