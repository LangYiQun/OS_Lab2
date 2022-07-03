#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>

#define __NR_init_graphics	95

_syscall0(int,init_graphics);

int main()
{
    init_graphics();
    while(1){};
    return 0;
}