/* CMPSC 170
 0;136;0c  Lab 1: Jshell
   Aimee Galang & Sean Spearman */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Command.h"

Command *head; //head of the linked list of commands

int main()
{

  /* Scanf is used to grab a string of stdin
     until a newline character is reached. 
     The string is stored in a character array
     named buffer. Length is also kept track
     of to check for if an & is the last
     character */

  char buffer[256];

  printf("Jshell: ");
  scanf("%[^\n]",buffer);

  printf("%s\n",buffer);

  /* First element of the linked list is
     created for the first command */

  Command *command= (Command *)malloc(sizeof(Command));
  head= command;

  if(command== NULL)
    {
      perror("ERROR: Dynamic memory allocation of a command element in the linked list failed.\n");
      //NEED ERROR EXIT PROPERLY
    }
  command->newargv= (char **)malloc(sizeof(char *)*256);
  command->next= NULL;
  command->input_file= NULL;
  command->output_file= NULL;
  command->append_file= NULL;
  command->background= 0;

  /* Strtok() is used to parse the string
     buffer by white-spaces. */

  char *word;
  int i= 0;

  word= strtok(buffer, " ");

  /* Exit Jshell if CTRL-D is hit/stdin closed
     or if 'exit' was typed as the first
     command */

  //EXIT CODE HERE

  while(word!= NULL)
    {
      printf("%s\n",word);//TEST: print word by word

      /* I/O redirection characters are checked
	 for. If they are found, the word after the
	 the I/O character is stored into
	 the command's element in the linked list. */

      if(!strcmp(word,"<"))
	{
 	  word= strtok(NULL, " ");
	  command->input_file= (char *)malloc(sizeof(char)*strlen(word));
	  strcpy(command->input_file,word);

 	  printf("input file: %s\n",word);

 	  word= strtok(NULL, " ");
	  continue;
	}
      if(!strcmp(word,">"))
	{
 	  word= strtok(NULL, " ");
	  command->output_file= (char *)malloc(sizeof(char)*strlen(word));
	  strcpy(command->output_file,word);

	  printf("output file: %s\n",word);

 	  word= strtok(NULL, " ");
	  continue;
	}
      if(!strcmp(word,"<<"))
	{
 	  word= strtok(NULL, " ");
	  command->append_file= (char *)malloc(sizeof(char)*strlen(word));
	  strcpy(command->append_file,word);

	  printf("append file: %s\n",word);

 	  word= strtok(NULL, " ");
	  continue;
	}

      /* A pipe is found so a new element
	 of the linked list is created to
	 hold new command information */

      if(!strcmp(word,"|"))
	{
	  /* Checks for an & at the end of
	     each command and flags the command
	     elements variable named 'background' */
	  if(!strcmp(command->newargv[i-1],"&"))
	    {

	      printf("& override argv[%i]= %s\n",i-1,command->newargv[i-1]);

	      command->newargv[i-1]= NULL;
	      command->background= 1;
	    }
	    else
	      command->newargv[i]= NULL;
	  i= 0;
	  Command *new= (Command *)malloc(sizeof(Command));
	  if(new== NULL)
	    {
	      perror("ERROR: Dynamic memory allocation of a command element in the linked list failed.\n");
	      //NEED ERROR EXIT PROPERLY
	    }
	  command->next= new;
	  command= new;
	  command->newargv= (char **)malloc(sizeof(char *)*256);
	  command->input_file= NULL;
	  command->output_file= NULL;
	  command->append_file= NULL;
	  command->background= 0;

	  printf("New linked list element\n");

	  word= strtok(NULL, " ");
	  continue;
	}

      /* If a word is not a <,> or << then
	 it is either a command name or a
	 command argument or flag. */

      char *mem= (char *)malloc(sizeof(char)*strlen(word));
      strcpy(mem,word);
      command->newargv[i]= mem;
      printf("argv[%i]: %s\n",i,command->newargv[i]);
      i++;

      word= strtok(NULL, " ");
    }

  /* Checks for an & at the end of
     each command and flags the command
     elements variable named 'background' */

  if(!strcmp(command->newargv[i-1],"&"))
    {

      printf("& override argv[%i]= %s\n",i-1,command->newargv[i-1]);

      command->newargv[i-1]= NULL;
      command->background= 1;
    }
    else
      command->newargv[i]= NULL;

  /* Fork and Execute */
  // NOTE: my code assumes use of the pipeline struct. 
  pid_t childpid;
  int status;
  int pipefd[2];
  int pipeout, pipein, nextin, inputfd;
  int commandnum = 1;

  // Loop through all commands
  Command *travel;
  for (travel = pipeline->head; travel->next != NULL; travel = travel->next) {
    // If this is not the last command in the pipeline, call pipe.
    if (/* this is not the last command in the pipeline*/ 1) {
      if (-1 == pipe(pipefd)) {
	perror("pipe");
        exit(EXIT_FAILURE);
      }
      pipein = pipefd[0];
      pipeout = pipefd[1];
    }
    // Fork child process. 
    childpid = fork();
    if (-1 == childpid) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    // If this is the child process, 
    if (0 == childpid) {
      // If first command in pipeline
      if (1 == commandnum) { // Was going to do *travel == *head but it made me uneasy
	// If there is input redirection
	if (NULL != input_file) { // Unsure if this will work, could just have a boolean inputRedirection var in Command struct if it causes problems.
	  if (-1 == close(stdin)) // Close stdin 
	    perror("close");
	  if (-1 == open(input_file, O_RDONLY)) // Open input_file, should replace stdin
	    perror("open");
	}
      } else {
	if (-1 == close(stdin)) // Close stdin 
	  perror("close");
	if (-1 == dup(stdin, pipein)) // Duplicate stdin to 
	  perror("dup");
	if (-1 == close(pipein)) 
	  perror("close");
      }
      // If last command in pipeline
      if (pipeline->length == commandnum) {
	
      }
    }
    ++commandnum;
  }

  return 0;
}
