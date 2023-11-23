#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

int read_length(int f1)
{
  char buf[10];
  int weight;
  int size = read(f1, buf, 4);

  if (size == -1) {
    perror("eroare citire");
    close(f1);
    exit(-1);
  }

  weight = *(int*)buf;
  return weight;
}

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

  //write(f2, string2, size);
  printf("%s\n", string2);
}

char *secondToDateTime(long s)
{
  struct tm *timeinfo;
  char *buff = (char*)malloc(21 * sizeof(char));

  if(buff == NULL)
    {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

  timeinfo = gmtime(&s);

  if(timeinfo == NULL)
    {
      perror("gmtime");
      free(buff);
      exit(EXIT_FAILURE);
    }

  strftime(buff, 21, "%Y-%m-%d %H:%M:%S", timeinfo);

  return buff;
}

int main(int argc, char **argv)
{
  printf("%s\n", argv[1]);
  struct stat fis;
  stat(argv[1], &fis);
  int f1 = open(argv[1], O_RDONLY);
  if (lseek(f1, 18, SEEK_SET) == -1) {
    perror("Error seeking in file");
    exit(-1);
  }

  int weight = read_length(f1);
  int height  = read_length(f1);

  char string[255];
  strcpy(string, "");

  int size = snprintf(string, sizeof(string), "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\ncontorul de legaturi: %ld\n", argv[1], height, weight, fis.st_size, fis.st_uid, secondToDateTime(fis.st_mtim.tv_sec), fis.st_nlink);

  //write(f2, string, size);
  printf("%s\n", string);
  drepturi(fis);
  close(f1);

  return 0;
}
