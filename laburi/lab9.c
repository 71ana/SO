#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int pfd[2];
  int pid;

  if(pipe(pfd) < 0)
    {
      perror("Eroare la crearea pipe-ului\n");
      exit(1);
    }

  if((pid = fork()) < 0)
    {
      perror("Eroare la fork\n");
      exit(1);
    }

  if(pid == 0)
    {
      char c;
      close(pfd[0]);
      scanf("%c", &c);
      write(pfd[1], &c, 1);
      close(pfd[1]);
      exit(0);
    }

  char buf;
  close(pfd[1]);
  read(pfd[0], &buf, 1);
  printf("%c\n", buf);
  close(pfd[0]);

  return 0;
}
