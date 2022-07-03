#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void main(int argc, char *argv[]){
  int pid;
  int status;
  char dirname[80];
  char cmdstr[255];
  int argc_cmd = 0;
  char *argv_cmd[10];
  char *p;

  while (1){
    /* prompt */
    getcwd(dirname, 80);
    printf("%s$ ", dirname);
    fflush(stdout);

    /* get command */
    memset(cmdstr, 0, 255);
    fgets(cmdstr, 255, stdin);
	
    /* parse command */
    argc_cmd = 0;
    p = strtok(cmdstr, " \n\t");
    while (p && argc_cmd < 9){
      argv_cmd[argc_cmd] = p;
      p = strtok(NULL, " \n\t");
      argc_cmd++;
    }
    argv_cmd[argc_cmd] = NULL;

    /* interpret internal commands */
    if (argc_cmd == 0)
      continue;
    if (strcmp(argv_cmd[0], "exit") == 0)
      break;
    if (strcmp(argv_cmd[0], "sync") == 0){
      sync();
      continue;
    }
    if (strcmp(argv_cmd[0], "cd") == 0){
      if ((argc_cmd == 1) || (strcmp(argv_cmd[1], "~") == 0))
        argv_cmd[1] = "/";
      if (chdir(argv_cmd[1]) < 0)
        printf("cd error\n");
      continue;
    }

    /* execute external commands */
    if (!(pid = fork())){
      if (execvp(argv_cmd[0], argv_cmd) == -1)
        printf("execvp error\n");
      exit(1);
    }
    while (1)
      if (pid == wait(&status))
        break;
  }

  /* exit */
  exit(0);
}
