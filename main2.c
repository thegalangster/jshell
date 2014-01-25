/* CMPSC 170
   Lab 1: Jshell
   Aimee Galang & Sean Spearman */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Info{

  char *input_file;
  char *output_file;
  char *append_file;

  char **newargv;

  struct Info *next;

};

int main()
{

  struct Info *head; // head of the linked list of commands
  int background= 0; // flag if we want to run line in background

  while(1)
    {

  char buffer[256];

  /* Fgets is used to grab a string of stdin
     until a newline character is reached. 
     The string is stored in a character array
     named buffer. */

  printf("jshell: ");
  fflush(stdin);
  if(fgets(buffer,256,stdin)== NULL) // CTRL-D exit
    return 0;
  buffer[strlen(buffer)-1]= '\0';

  /* First element of the linked list is
     created for the first command */

  struct Info *command= (struct Info *)malloc(sizeof(struct Info));
  head= command;
  if(command== NULL)
    {
      fprintf(stderr,"ERROR: malloc allocation failed\n");
      exit(1);
    }
  command->newargv= (char **)malloc(sizeof(char *)*256);
  if(command->newargv== NULL)
    {
      fprintf(stderr,"ERROR: malloc allocation failed\n");
      exit(1);
    }
  command->newargv[0]= NULL;
  command->next= NULL;
  command->input_file= NULL;
  command->output_file= NULL;
  command->append_file= NULL;

  /* Strtok() is used to parse the string
     buffer by white-spaces. */

  char *word;
  int i= 0;
  int redo= 0; // flags a syntax error

  word= strtok(buffer, " ");
  while(word!= NULL)
    {
      if(!strcmp(word,"exit")) // exit if 'exit' is the first word
	return 0;

      /* Error Checking
	 Check for ||, &&, if, while */

      if(!strcmp(word,"||")||!strcmp(word,"&&")||!strcmp(word,"if")||!strcmp(word,"while"))
	{
	  fprintf(stderr,"ERROR: no support for input shell built-in command %s\n",word);
	  redo= 1;
	  break;
	}

      /* I/O redirection characters are checked
	 for. If they are found, the word after the
	 the I/O character is stored into
	 the command's element in the linked list. */

      if(!strcmp(word,"<"))
	{
 	  word= strtok(NULL, " ");

	  /* Error Checking
	     Check stray < with no following word as file input */

	  if((word== NULL)||(!strcmp(word,"|")))
	    {
	      fprintf(stderr,"ERROR: no input file inputted after <\n");
	      redo= 1;
	      break;
	    }

	  command->input_file= (char *)malloc(sizeof(char)*strlen(word));
	  if(command->input_file== NULL)
	    {
	      fprintf(stderr,"ERROR: malloc allocation failed\n");
	      exit(1);
	    }
	  strcpy(command->input_file,word);

 	  printf("input file: %s\n",word);

 	  word= strtok(NULL, " ");
	  continue;
	}
      if(!strcmp(word,">"))
	{
 	  word= strtok(NULL, " ");

	  /* Error Checking
	     Check stray > with no following word as file output */

	  if((word== NULL)||(!strcmp(word,"|")))
	    {
	      fprintf(stderr,"ERROR: no output file inputted after >\n");
	      redo= 1;
	      break;
	    }

	  command->output_file= (char *)malloc(sizeof(char)*strlen(word));
	  if(command->output_file== NULL)
	    {
	      fprintf(stderr,"ERROR: malloc allocation failed\n");
	      exit(1);
	    }
	  strcpy(command->output_file,word);

	  printf("output file: %s\n",word);

 	  word= strtok(NULL, " ");
	  continue;
	}
      if(!strcmp(word,"<<"))
	{
 	  word= strtok(NULL, " ");

	  /* Error Checking
	     Check stray << with no following word as file to append */

	  if((word== NULL)||(!strcmp(word,"|")))
	    {
	      fprintf(stderr,"ERROR: no file to append inputted after <<\n");
	      redo= 1;
	      break;
	    }

	  command->append_file= (char *)malloc(sizeof(char)*strlen(word));
	  if(command->append_file== NULL)
	    {
	      fprintf(stderr,"ERROR: malloc allocation failed\n");
	      exit(1);
	    }
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

	  i= 0;
	  struct Info *new= (struct Info *)malloc(sizeof(struct Info));
	  if(new== NULL)
	    {
	      fprintf(stderr,"ERROR: malloc allocation failed\n");
	      exit(1);
	    }
	  command->next= new;
	  command= new;
	  command->newargv= (char **)malloc(sizeof(char *)*256);
	  if(command->newargv== NULL)
	    {
	      fprintf(stderr,"ERROR: malloc allocation failed\n");
	      exit(1);
	    }
	  command->newargv[0]= NULL;
	  command->input_file= NULL;
	  command->output_file= NULL;
	  command->append_file= NULL;

	  printf("New linked list element\n");

	  word= strtok(NULL, " ");
	  continue;
	}

      /* Error Checking
	 Check if there are multiple '|'s without spaces */

      if(strchr(word,'|')!= NULL)
	{
	  fprintf(stderr, "Error: mulitple '|'s with no commands\n");
	  redo= 1;
	  break;
	}

      /* Checks for & as the last character in a line and
	 sets background= 1 for the command if so */

      char *hold= word;
      word= strtok(NULL, " ");
      if((word== NULL)&&(!strcmp(hold,"&")))
	{
	  background= 1;
	  printf("runs in background\n");
	  break;
	}

      /* Error Checking
	 Checks for &s that are not at the end of a line and exits */

      else
	{
	  if(!strcmp(hold,"&"))
	    {
	      fprintf(stderr,"ERROR: & in invalid location\n");
	      redo= 1;
	      break;
	    }      
	}

      /* If a word is not a &, <, > or << then
	 it is either a command name or a
	 command argument or flag. */

      char *mem= (char *)malloc(sizeof(char)*strlen(hold));
      if(mem== NULL)
	{
	  fprintf(stderr,"malloc allocation failed\n");
	  exit(1);
	}
      strcpy(mem,hold);
      command->newargv[i]= mem;
      printf("argv[%i]: %s\n",i,command->newargv[i]);
      i++;
    }

  /* Checks if 'redo' syntax error is flagged and
     jshell re-asks for a new correct line to be
     inputted */

  if(redo)
    {
      redo= 0;

      /* De-allocate Memory */

      struct Info *temp1;
      for(temp1= head;temp1!= NULL;temp1= temp1->next)
	{
	  free(temp1->newargv);
	  free(temp1);
	}
      continue;
    }

  command->newargv[i]= NULL;

  /* Error Checking
     Checks for empty newargv array (no commands and arguments) */

  struct Info *temp1;
  for(temp1= head;temp1!= NULL;temp1= temp1->next)
    {
      if(temp1->newargv[0]== NULL)
	{
	  fprintf(stderr,"ERROR: no command\n");
	  redo= 1;
	  break;
	}
    }

  /* Checks if 'redo' syntax error is flagged and
     jshell re-asks for a new correct line to be
     inputted */

  if(redo)
    {
      redo= 0;

      /* De-allocate Memory */

      struct Info *temp2;
      for(temp2= head;temp2!= NULL;temp2= temp2->next)
	{
	  free(temp2->newargv);
	  free(temp2);
	}
      continue;
    }

  /* FORK AND EXECUTE HERE */

  int pipeout, nextin;
  int pipefd[2];

  /* Loop through commands */

  struct Info temp;
  for(temp= head; temp!= NULL; temp= temp->next)
    {

      /* If this is not the last command in the pipeline:
	 Call pipe to allcoate two file descriptors for the pipe
	 and save one as 'pipeout' and 'nextin' */

      if(temp->next!= NULL)
	{

	  int i= pipe(pipefd);
	  if(i< 0)
	    {
	      perror("Pipe error");
	      exit(1);
	    }



	}

    } // end loop through commands


  /* De-allocate Memory */

  struct Info *temp1;
  for(temp1= head;temp1!= NULL;temp1= temp1->next)
    {
      free(temp1->newargv);
      free(temp1);
    }
    }

  return 0;
}



  /* Fork and Execute */
  // NOTE: my code assumes use of the pipeline struct. 
  /*
  pid_t childpid;
  int status;
  int pipefd[2];
  int pipeout, pipein, nextin, inputfd;
  int commandnum = 1;

  // Loop through all commands
  Command *travel;
  for (travel = pipeline->head; travel->next != NULL; travel = travel->next) {
    // If this is not the last command in the pipeline, call pipe.
    if (commandnum != pipeline->length) {
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
    // If this is the child process 
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
	if (-1 == close(stdin)) 
	  perror("close");
	if (-1 == dup(pipein)) // Make duplicate of pipein at stdin
	  perror("dup");
	if (-1 == close(pipein)) // Close other pipein since stdin now is fd for it 
	  perror("close");
      }
      // If last command in pipeline
      if (pipeline->length == commandnum) {
	// If there is output overwrite redirect
	if (NULL != output_file) {
	  if (-1 == close(stdout)) 
	    perror("close");
	  if (-1 == open(output_file, O_WRONLY))
	    perror("open");
	}
	// If there is output append redirect
	else if (NULL != append_file) {	
	  if (-1 == close(stdout)) 
	    perror("close");
	  if (-1 == open(output_file, O_APPEND))
	    perror("open");
	}
      } else {
	if (-1 == close(nextin))
	  perror("close");
	if (-1 == close(stdout))
	  perror("close");
	if (-1 == dup(pipeout)) 
	  perror("dup");
	if (-1 == close(pipeout)) 
	  perror("close");
      }
      if(-1 == execvp(travel->newargv[0], travel->newargv)) // This won't compile b/c newargv ain't const.
	perror("execvp");
    } else { // Else it is the parent process (Jshell)
      // Record the child PID here (what?)
      
      // If this is NOT the first process in the pipeline
      if (1 != commandnum) {
	// Close pipein since child has it
	if (-1 == close(pipein))
	  perror("close");
      }
      // If this is NOT the last process in the pipeline
      if (pipeline->length != commandnum) {
	// Could alternatively be written(?): (close(pipeout) == -1) ? perror("close");
	if (-1 == close(pipeout))
	  perror("close");
      }
    }
    ++commandnum;
  }
*/
