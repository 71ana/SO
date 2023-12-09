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
  
  
  int width = read_length(f1);
  int height  = read_length(f1);

  char string[255];
  strcpy(string, "");

  int size = sprintf(string,"nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %scontorul de legaturi: %ld\n", name, height, width, fis.st_size, fis.st_uid, ctime(&fis.st_mtim.tv_sec), fis.st_nlink);

  write(out, string, size);
  drepturi(fis, out);
  close(out);
  close(f1);
  exit(10);
}

void bmp_convert(char argv[100], char name[100]) {
    char path[500];
    strcpy(path, "");
    sprintf(path, "%s/%s", argv, name);
    
    int f = open(path, O_RDWR);
    if (lseek(f, 18, SEEK_SET) == -1) {
      perror("Error seeking in file");
      exit(-1);
    }
    
    int width = read_length(f);
    int height = read_length(f);
    
    if (f == -1) {
        perror("eroare deschidere fisier");
        exit(1);
    }

    if (lseek(f, 36, SEEK_SET) == -1) {
        perror("Error seeking in file");
        exit(-1);
    }

    int size = height*width;
    
    char color[3];
    for (int i = 0; i < size; i++)
      {
	if (read(f, color, 3) == -1) {
	  perror("eroare citire pixel\n");
	  close(f);
	  exit(1);
	}
	
	char gray = 0.299 * color[0] + 0.587 * color[1] + 0.114 * color[2];
	color[0] = gray;
	color[1] = gray;
	color[2] = gray;
	
	if (lseek(f, -3, SEEK_CUR) == -1) {
	  perror("eroare mutare inapoi\n");
	  close(f);
	  exit(1);
	}
	
	if (write(f, color, 3) == -1) {
	  perror("eroare scriere in fisier\n");
	  close(f);
	  exit(1);
	}
      }
    
    close(f);
    exit(13);
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
  if(arg != 4)
    {
      printf("Usage %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
      exit(-1);
    }

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

		  int pid2 = fork();
		  if( pid2 < 0)
		    {
		      perror("Eroare");
		      exit(1);
		    }
		  if(pid2 == 0)
		    {
		      bmp_convert(argv[1], entry->d_name);
		    }
		}
	      else
		{
		  //fisier normal
		  int pfd[2]; //pipe pentru transmiterea intre copii
		  
		  int pid;
		  char c = argv[3][0]; //caracterul pe care il cautam in propozitii
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
		      char string[500];
		      close(pfd[0]);
		      strcpy(string, "");
		      
		      int size = snprintf(string, sizeof(string), "nume fisier: %s\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\ncontorul de legaturi: %ld\n", entry->d_name, fis.st_size, fis.st_uid, ctime(&fis.st_mtim.tv_sec), fis.st_nlink);
		      write(f2, string, size);
		      drepturi(fis, f2);
		      close(f2);
		      dup2(pfd[1], 1);
		      close(pfd[1]);
		      
		      execlp("cat", "cat", path, NULL);
		      perror("eroare executie cat\n");
       		      exit(8);
		    }
		  
		  int pfd2[2]; //pipe pentru transmiterea copil-parinte
		  if(pipe(pfd2) < 0)
		    {
		      perror("Eroare la crearea pipe-ului 2\n");
		      exit(1);
		    }
		  int pid2 = fork();
		  if( pid2 < 0 )
		    {
		      perror("Eroare");
		      exit(1);
		    }
		  if(pid2 == 0)
		    {
		      close(pfd[1]); //urmeaza a citi din primul pipe ce e in fisier
		      close(pfd2[0]);  //urmeaza a scrie in pipe numarul de linii corecte care contin litera data ca argument
		      
		      dup2(pfd[0], 0); //intrarea standard
		      close(pfd[0]);
		      
		      dup2(pfd2[1], 1); //iesirea standard
		      close(pfd2[1]);
		      
		      execlp("bash", "bash" , "script.sh", c, NULL);
		      exit(20);
		    }
		  
		  close(pfd2[1]);
		  close(pfd[0]);
		  close(pfd[1]);
		  
		  char val[3];
		  read(pfd2[0], val, sizeof(val));
		  val[strlen(val)-1]='\0';
		  printf("%s\n", val);
		  int nr = atoi(val);
		  printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c.\n", nr, c);
		  close(pfd2[0]);
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
	  printf("S-a incheiat procesul cu pid-ul %d si codul %d.\n", wpid, WEXITSTATUS(status));
	}
    }
  
  closedir(dir);
  close(f2);
  
  return 0;
}
