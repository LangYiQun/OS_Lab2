#include <stdio.h>
#include <stdlib.h>

#define NAME_MAX 80
#include <dirent.h>

int main(int argc, char *argv[]) 
{
  DIR *dir ; 
  struct dirent *de;

  if (argc != 1)
    exit(1);
   
  if ((dir = opendir(".")) == NULL)
    exit(1);

  while ((de = readdir(dir)) != NULL)
    printf("%s\t", de->d_name);
   
  printf("\n");
  closedir(dir);
  exit(0);
}
