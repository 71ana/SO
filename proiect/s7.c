#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

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

void drepturi(struct stat fis, int f2)
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


  int size = sprintf(string2, "drepturi de acces user: %s\ndrepturi de acces grup: %s\ndrepturi de acces altii: %s", user, grup, others);

  write(f2, string2, size);
}


int main(int arg, char *argv[])
{
  if(arg != 2)
    {
      printf("Usage %s %s\n", argv[0], argv[1]);
      exit(-1);
    }

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
	  strcat(path, argv[1]);
	  strcat(path, "/");
	  strcat(path, entry->d_name);

	  struct stat fis;
	  stat(path, &fis);

	  if(S_ISREG(fis.st_mode))
	    {
	      if(strstr(entry->d_name, ".bmp") != 0)
		{
	      //fisier obisnuit cu extensia .bmp
		  int f1 = open(path, O_RDONLY);
		  if (lseek(f1, 18, SEEK_SET) == -1) {
		    perror("Error seeking in file");
		    exit(-1);
		  }

		  int weight = read_length(f1);
		  int height  = read_length(f1);

		  char string[255];

		  int size = sprintf(string, "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %ld\ncontorul de legaturi: %ld\n", argv[1], height, weight, fis.st_size, fis.st_uid, fis.st_mtim.tv_sec, fis.st_nlink);

		  write(f2, string, size);

		  drepturi(fis, f2);
		  close(f1);
		}
	    }
	  else
	    {
	      //fisier normal

	      char string[255];

	      strcpy(string, "");

	      int size = sprintf(string, "nume fisier: %s\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %ld\ncontorul de legaturi: %ld\n", argv[1], fis.st_size, fis.st_uid, fis.st_mtim.tv_sec, fis.st_nlink);

	      write(f2, string, size);

	      drepturi(fis, f2);
	    }

	  if(S_ISLNK(fis.st_mode))
	    {
	      char string[255];
	      strcpy(string, "");

	      struct stat link;
	      lstat(path, &link);

	      int size = sprintf(string, "nume legatura: %s\ndimensiune legatura: %ld\ndimensiune fisier dimensiunea fisierului target: %ld\n", entry->d_name, link.st_size, link.st_size);

	      write(f2, string, size);
	      drepturi(fis, f2);

	    }

	  if(S_ISDIR(fis.st_mode))
	    {
	      char string[255];
	      strcpy(string, "");

	      int size = sprintf(string, "nume director: %s\n, identificatorul utilizatorului: %d\n", entry->d_name, fis.st_uid);
	      write(f2, string, size);
	      drepturi(fis, f2);
	    }

	}
    }








  closedir(dir);
  close(f2);

  return 0;
}
