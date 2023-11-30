#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <wait.h>
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

void drepturi(struct stat fis, int out)
{
  char string[255], user[4], grup[4], others[4];

  //user
  strcpy(user, "");
  if (fis.st_mode & S_IRUSR)  { strcat(user, "R"); }
  else  { strcat(user, "-"); }

  if (fis.st_mode & S_IWUSR)  { strcat(user, "W"); }
  else  { strcat(user, "-"); }

  if (fis.st_mode & S_IXUSR)  { strcat(user, "X"); }
  else  {  strcat(user, "-"); }


  //grup
  strcpy(grup, "");
  if (fis.st_mode & S_IRGRP)  { strcat(grup, "R"); }
  else  {  strcat(grup, "-"); }

  if (fis.st_mode & S_IWGRP)  { strcat(grup, "W"); }
  else  {  strcat(grup, "-"); }

  if (fis.st_mode & S_IXGRP)  {  strcat(grup, "X"); }
  else  {  strcat(grup, "-"); }


  //altii
  strcpy(others, "");
  if (fis.st_mode & S_IROTH)  {  strcat(others, "R"); }
  else  {  strcat(others, "-"); }

  if (fis.st_mode & S_IWOTH)  {  strcat(others, "W"); }
  else  {  strcat(others, "-"); }

  if (fis.st_mode & S_IXOTH)  {  strcat(others, "X"); }
  else  {  strcat(others, "-"); }


  int size = sprintf(string, "drepturi de acces user: %s\ndrepturi de acces grup: %s\ndrepturi de acces altii: %s\n\n", user, grup, others);

  write(out, string, size);
}

void is_fisbmp(char argv[100], char name[100], int out)
{
  char path[500];
  strcpy(path, "");
  sprintf(path, "%s/%s", argv, name);
  struct stat fis;
  lstat(path, &fis);

  int f1 = open(path, O_RDONLY);
  if (lseek(f1, 18, SEEK_SET) == -1) {
    perror("Error seeking in file");
    exit(-1);
  }


  int weight = read_length(f1);
  int height  = read_length(f1);

  char string[255];
  strcpy(string, "");

  int size = sprintf(string,"nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\n", name, height, weight, fis.st_size, fis.st_uid, ctime(&fis.st_mtim.tv_sec), fis.st_nlink);

  write(out, string, size);
  drepturi(fis, out);
  close(out);
  exit(10);
}

void is_fis(struct stat fis, char name[100], int out)
{
  //fisier normal
  char string[500];
  strcpy(string, "");

  int size = snprintf(string, sizeof(string), "nume fisier: %s\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\ncontorul de legaturi: %ld\n", name, fis.st_size, fis.st_uid, ctime(&fis.st_mtim.tv_sec), fis.st_nlink);

  write(out, string, size);
  drepturi(fis, out);
  close(out);
  exit(9);
}

void is_dir(struct stat fis, char name[100], int out)
{
  //director
  char string[500];
  strcpy(string, "");

  int size = snprintf(string, sizeof(string), "nume director: %s\nidentificatorul utilizatorului: %d\n", name, fis.st_uid);
  write(out, string, size);

  drepturi(fis, out);
  close(out);
  exit(5);
}

void is_link(struct stat fis, char name[100], int out)
{
  //legatura
  char string[255];
  strcpy(string, "");

  int size = snprintf(string, sizeof(string), "nume legatura: %s\ndimensiune legatura: %ld\ndimensiune fisier dimensiunea fisierului target: %ld\n", name, fis.st_size, fis.st_size);

  write(out, string, size);
  drepturi(fis, out);
  close(out);
  exit(6);
}

int main(int arg, char *argv[])
{
  if(arg != 3)
    {
      printf("Usage %s %s %s\n", argv[0], argv[1], argv[2]);
      exit(-1);
    }
  printf("ok\n");

  DIR *dir = opendir(argv[1]);

  if(dir == NULL)
    {
      perror("eroare deschidere director intrare\n");
      exit(EXIT_FAILURE);
    }

  DIR *dir2 = opendir(argv[2]);

  if(dir2 == NULL)
    {
      perror("eroare deschidere director iesire\n");
      exit(EXIT_FAILURE);
    }

  char out_path[500];
  int f2;

  struct dirent *entry;
  while((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
	{
	  char path[500];
	  strcpy(path, "");
	  sprintf(path, "%s/%s", argv[1], entry->d_name);
	  struct stat fis;
	  lstat(path, &fis);

	  strcpy(out_path, "");
	  sprintf(out_path, "%s/%s_statistica.txt", argv[2], entry->d_name);

	  f2 = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

	  if(S_ISREG(fis.st_mode))
	    {
	      if(strstr(entry->d_name, ".bmp") != NULL)
		{
		  int pid = fork();
		  if( pid < 0 )
		    {
		      perror("Eroare");
		      exit(1);
		    }
		  if(pid == 0)
		    {
		      is_fisbmp(argv[1], entry->d_name, f2);
		    }
		}
	      else
		{
		  //fisier normal
		  int pid = fork();
		  if( pid < 0 )
		    {
		      perror("Eroare");
		      exit(1);
		    }
		  if(pid == 0)
		    {
		      is_fis(fis, entry->d_name, f2);
		    }
		}
	    }

	  if(S_ISLNK(fis.st_mode))
	    {
	      int pid = fork();
	      if( pid < 0 )
		{
		  perror("Eroare");
		  exit(1);
		}
	      if(pid == 0)
		{
		  is_link(fis, entry->d_name, f2);
		}
	    }

	  if(S_ISDIR(fis.st_mode))
	    {
	      int pid = fork();
	      if( pid < 0 )
		{
		  perror("Eroare");
		  exit(1);
		}
	      if(pid == 0)
		{
		  is_dir(fis, entry->d_name, f2);
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
