#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MSG_LEN 256
int main(int argc, char **argv)
{
  int num, fd;
  char msg[MSG_LEN+1];

  if (argc == 1)
    return EXIT_FAILURE;

  /* open the file */
  if ((fd=open(argv[1], O_RDONLY, 0)) < 0)
    return EXIT_FAILURE;
  do{
    /* read the file */
    if ((num = read(fd, msg, MSG_LEN)) < 0) {
      close(fd);
      return EXIT_FAILURE;
    } else if (num == 0) {
      break;
    }
    /* display on screen */
    msg[num] = '\0';
    printf("%s", msg);
  }while(1);
  printf("\n");

  /* exit */
  close(fd);
  sync();

  return EXIT_SUCCESS;
}
