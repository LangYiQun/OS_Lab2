#define __LIBRARY__
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define	EINVAL	4

extern char ** environ;
#define	__environ	environ

char * ___strtok;
volatile int errno;

_syscall1(int,setup,void *,BIOS)

volatile void exit(int exit_code)
{
	__asm__("int $0x80"::"a" (__NR_exit),"b" (exit_code));
	while(1); /* to avoid warning of compiler */
}

extern char _end;
void * ___brk_addr = &_end;

int brk(void * end_data_segment)
{
	long __res;
	__asm__ volatile ("int $0x80"
		: "=a" (__res)
		: "0" (__NR_brk),"b" ((long)(end_data_segment)));
	if (__res >= 0) {
		___brk_addr = end_data_segment;
		return (int) __res;
	}
	errno = -__res;
	return -1;
}

void * sbrk(ptrdiff_t increment)
{
	char *oldbrk;

	if (increment == 0)
		return ___brk_addr;

	oldbrk = ___brk_addr;
	if (brk(oldbrk + increment) < 0)
		return (char *) -1;

	return oldbrk;
}

/* Return the value of the environment variable NAME.  */
char * getenv(register const char *name)
{
  register const size_t len = strlen(name);
  register char **ep;

  for (ep = __environ; *ep != NULL; ++ep)
    if (!strncmp(*ep, name, len) && (*ep)[len] == '=')
      return &(*ep)[len + 1];

  return NULL;
}
