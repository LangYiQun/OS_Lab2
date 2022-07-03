#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/segment.h>
#include <sys/stat.h>


int sys_sleep(unsigned int seconds){
	//收到SIGALRM信号，则忽略，啥也不干
	sys_signal(SIGALRM , SIG_IGN);
	//设定一个时长为seconds秒的闹钟
	sys_alarm(seconds);
	/*
	系统调用pause函数，来暂停当前进程，当前进程进入睡眠状态。
	直到接收到信号且信号函数成功返回，pause函数才会返回，而且返回值为-1。
	*/
	sys_pause();
	return 0; 
}