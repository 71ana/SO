 #include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
  int pid;
  if( ( pid=fork() ) < 0)
    {
    perror("Eroare");
    exit(1);
    }

  if(pid==0)
    {
      /* codul fiului - doar codul fiului poate ajunge aici (doar in fiu fork() returneaza 0)*/

      execl("./out", "studenti", "6", NULL);

      exit(0); // apel necesar pentru a se opri codul fiului astfel incat acesta sa nu execute si codul parintelui
    }
  //codul parintelui
  while(1)
    {
      char string[50];
      scanf("%s", string);
      printf("%s\n", string);
      sleep(500);
    }
  int status;
  int wpid = wait(&status);
  if(WIFEXITED(status))
    {
      printf("ok\n");
    }

  return 0;
}
