#include <stdio.h>
#include <string.h>
//#include "fields.h"

int main()
{
  int len = 0;
  char buffer[256];

  char *commands[256];

  printf("Jshell: ");
  
  scanf("%[^\n]",buffer);
  len = strlen(buffer);
  char *split;
  int i= 0;
  split= strtok(buffer, "|");
  while(split!= NULL)
    {
      printf("%s\n",split);
      buffer2[i]= split;
      i++;
      split= strtok(NULL, "|");
      printf("word:%s ",buffer);
    }


  return 0;

}
