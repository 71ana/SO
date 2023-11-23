#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <wait.h>


int main(int arg, char *argv[])
{
  if(arg != 2)
    {
      printf("Usage %s %s\n", argv[0], argv[1]);
      exit(-1);
    }
  printf("ok\n");

  DIR *dir = opendir(argv[1]);

  if(dir == NULL)
    {
      perror("eroare deschidere director");
      exit(EXIT_FAILURE);
    }

  int f2 = open("statistica2.txt", O_RDWR | O_CREAT, S_IRWXU);

  if(f2 == -1)
    {
      perror("eroare la deschiderea fisierului\n");
      exit(-1);
    }

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
	{
	  char path[255];
	  strcpy(path, "");
	  //snprintf(path, sizeof(path), "%s/%s", argv[1], entry->d_name);

	  struct stat fis;
	  stat(entry->d_name, &fis);

	  pid_t pid = fork();
	  if( pid < 0 )
	    {
	      perror("Eroare");
	      exit(1);
	    }
	  if(pid == 0)
	    {
	      if(S_ISREG(fis.st_mode))
		{
		  execl("./fisbmp", "fisbmp", "entry->d_name", NULL);
		  exit(0);
		}
	      else
		{
		  //fisier normal
		  execl("./fis", "fis", "entry->d_name", NULL);
		  exit(0);
		}

	      if(S_ISLNK(fis.st_mode))
		{
		  execl("./leg", "leg", "entry->d_name", NULL);

		}

	      if(S_ISDIR(fis.st_mode))
		{
		  execl("./dir", "dir", "entry->d_name", NULL);
		  exit(0);
		}
	    }
	}
    }

  int status;
  int wpid;
  while ((wpid = wait(&status)) != -1)
    {
      if (WIFEXITED(status))
	{
	  printf("Child process %d completed with status %d\n", wpid, WEXITSTATUS(status));
	}
    }

  printf("ok!!!!\n");
  closedir(dir);
  close(f2);

  return 0;
}
