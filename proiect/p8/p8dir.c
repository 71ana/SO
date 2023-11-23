#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <wait.h>

void drepturi(struct stat fis)
{
  char string2[255], user[4], grup[4], others[4];

  //user
  strcpy(user, "");
  if (fis.st_mode & S_IRUSR)
    {
      strcat(user, "R");
    }
  else
    {
      strcat(user, "-");
    }

  if (fis.st_mode & S_IWUSR)
    {
      strcat(user, "W");
    }
  else
    {
      strcat(user, "-");
    }

  if (fis.st_mode & S_IXUSR)
    {
      strcat(user, "X");
    }
  else
    {
      strcat(user, "-");
    }


  //grup
  strcpy(grup, "");
  if (fis.st_mode & S_IRGRP)
    {
      strcat(grup, "R");
    }
  else
    {
      strcat(grup, "-");
    }

  if (fis.st_mode & S_IWGRP)
    {
      strcat(grup, "W");
    }
  else
    {
      strcat(grup, "-");
    }

  if (fis.st_mode & S_IXGRP)
    {
      strcat(grup, "X");
    }
  else
    {
      strcat(grup, "-");
    }


  //altii
  strcpy(others, "");
  if (fis.st_mode & S_IROTH)
    {
      strcat(others, "R");
    }
  else
    {
      strcat(others, "-");
    }

  if (fis.st_mode & S_IWOTH)
    {
      strcat(others, "W");
    }
  else
    {
      strcat(others, "-");
    }

  if (fis.st_mode & S_IXOTH)
    {
      strcat(others, "X");
    }
  else
    {
      strcat(others, "-");
    }


  int size = sprintf(string2, "drepturi de acces user: %s\ndrepturi de acces grup: %s\ndrepturi de acces altii: %s\n\n", user, grup, others);

  // write(f2, string2, size);
  printf("%s", string2);
}

int main(int argc, char **argv)
{

  //fisier normal
  char string2[255];
  strcpy(string2, "");

  struct stat fis;
  stat(argv[1], &fis);

  int size = snprintf(string2, sizeof(string2), "nume director: %s\nidentificatorul utilizatorului: %d\n", argv[1], fis.st_uid);
  //write(f2, string2, size);
  printf("%s\n", string2);
  drepturi(fis);

  return 0;
}
