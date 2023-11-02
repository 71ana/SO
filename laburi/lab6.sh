#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int arg, char *argv[])
{
  char buf[50];
  int litere_mici = 0;
  int litere_mari = 0;
  int cifre = 0;
  int aparitii_caracter = 0;
  struct stat dim;

  if(arg != 4)
    {
      printf("nu avem 3 parametri!\n");
      exit(EXIT_FAILURE);
    }

  int f1, f2;

  f1 = open(argv[1], O_RDONLY);
  if(f1 == -1)
    {
      perror("eroare deschidere fisier");
    }

  f2 = open(argv[2],O_WRONLY | O_CREAT);
  if(f1 == -1)
    {
      perror("eroare deschidere fisier");
    }

  int size;

  while ((size = read(f1, buf, 50)) != 0)
    {
      buf[size] = '\0';
      for(int i = 0 ; i < size; i++)
	{
	  if(islower(buf[i]) != 0)
	    litere_mici++;
	  if(isupper(buf[i]) != 0)
	    litere_mari++;
	  if(isdigit(buf[i]) != 0)
	    cifre++;
	  if(buf[i] == argv[3][0])
	    aparitii_caracter++;
	}
    }

  /*printf("%d\n", litere_mici);
  printf("%d\n", litere_mari);
  printf("%d\n", cifre);
  printf("%d\n", aparitii_caracter);
  printf("%ld\n", dim.st_size);
  */

  fstat(f1, &dim);

  char text[255];
  sprintf(text, "numar litere mici: %d\nnumar litere mari: %d\nnumar cifre: %d\nnumar aparitii caracter: %d\ndimensiune fisier: %ld\n", litere_mici, litere_mari, cifre, aparitii_caracter, dim.st_size);

  write(f2, text, strlen(text));

  close(f1);
  close(f2);

  return 0;
}
