/* CMPSC 170
   Lab 1: Jshell
   Aimee Galang & Sean Spearman */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


// Struct for keeping track of return statuses.
typedef struct Job Job;
struct Job {
  int status;
  pid_t pid;
  int background;
};

struct Info{

  char *input_file;
  char *output_file;
  char *append_file;
  char **newargv;
  struct Info *next;

};

int main(int argc, char **argv)
{

  struct Info *head= NULL; // head of the linked list of commands
  int background= 0; // flag if we want to run line in background
  char *prompt = "Jsh: ";

  int jobArrLen = 128;
  Job jobArr[jobArrLen];
  int jobi = 0;
  for (jobi = 0; jobi < jobArrLen; ++jobi) {
    jobArr[jobi].status = 0;
    jobArr[jobi].pid = 0;
    jobArr[jobi].background = 0;
  }
  int first = 1;

  /*
  Status *statHead = (Status *)malloc(sizeof(Status));
  statHead->next = NULL; // Not sure if necessary.
  Status *statTravel = statHead;
  Status *statTemp = statHead;
  */

  int waitValue;

  // Check if prompt argument passed
  if (argc == 2)
    prompt = argv[1];
  else if (argc > 2)
    fprintf(stderr, "USAGE: jshell [PROMPT]\nDon't try 'jshell --help'\nPROMPT set to 'Jsh: '\n");
  
  while(1)
    {
      
      char buffer[256];
      
      /* Wait for processes if necessary. */
      if (first == 0) {
	for (jobi = 0; jobi < jobArrLen; ++jobi) {
	  if (jobArr[jobi].pid == 0)
	    continue;
	  if (jobArr[jobi].background == 1) {
	    waitValue = waitpid(jobArr[jobi].pid, &(jobArr[jobi].status), WNOHANG);
	    if (waitValue == -1) {
	      perror("waitpid(jobArr[jobi].pid, &(jobArr[jobi].status), WNOHANG)");
	      exit(EXIT_FAILURE);
	    }
	    else if (waitValue > 0) {
	      jobArr[jobi].background = 0;
	      jobArr[jobi].pid = 0;
	      jobArr[jobi].status = 0;
	    } 
	  }
	  else {
	    waitValue = waitpid(jobArr[jobi].pid, &(jobArr[jobi].status), 0);
	    if (waitValue == -1) {
	      perror("waitpid(jobArr[jobi].pid, &(jobArr[jobi].status), 0)");
	      exit(EXIT_FAILURE);
	    }
	    else if (waitValue > 0) {
	      jobArr[jobi].background = 0;
	      jobArr[jobi].pid = 0;
	      jobArr[jobi].status = 0;
	    } 
	  }
	}
      }
      
      /* Fgets is used to grab a string of stdin
	 until a newline character is reached. 
	 The string is stored in a character array
	 named buffer. */
      printf("%s", prompt);
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
      background= 0;
      head= command;
      
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
	      
	      if((word== NULL)||(strchr(word,'|')!= NULL)||(!strcmp(word,">>"))||(!strcmp(word,">"))||(!strcmp(word,"<"))||(!strcmp(word,"&")))
		{
		  fprintf(stderr,"ERROR: invalid filename found after <\n");
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
	      
	      // 	  printf("input file: %s\n",word);
	      
	      word= strtok(NULL, " ");
	      continue;
	    }
	  if(!strcmp(word,">>"))
	    {
	      word= strtok(NULL, " ");
	      
	      /* Error Checking
		 Check stray << with no following word as file to append */
	      
	      if((word== NULL)||(strchr(word,'|')!= NULL)||(!strcmp(word,">>"))||(!strcmp(word,">"))||(!strcmp(word,"<"))||(!strcmp(word,"&")))
		{
		  fprintf(stderr,"ERROR: invalid filename found after >>\n");
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
	      
	      //	  printf("append file: %s\n",word);
	      
	      word= strtok(NULL, " ");
	      continue;
	    }
	  if(!strcmp(word,">"))
	    {
	      word= strtok(NULL, " ");
	      
	      /* Error Checking
		 Check stray > with no following word as file output */
	      
	      if((word== NULL)||(strchr(word,'|')!= NULL)||(!strcmp(word,">>"))||(!strcmp(word,">"))||(!strcmp(word,"<"))||(!strcmp(word,"&")))
		{
		  fprintf(stderr,"ERROR: invalid filename found after >\n");
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
	      
	      //	  printf("output file: %s\n",word);
	      
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
	      
	      //	  printf("New linked list element\n");
	      
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
	  
	  /* Checks for & as the last character in a line or before a "|"
	     command and sets background= 1 for the command if so */
	  
	  char *hold= word;
	  word= strtok(NULL, " ");
	  if((!strcmp(hold,"&"))&&(((word== NULL)||(!strcmp(word,"|")))&&(!strcmp(hold,"&"))))
	    {
	      background= 1;
	      //	  printf("runs in background\n");
	      continue;
	    }
	  
	  /* Error Checking
	     Checks for invalid &s and exits */
	  
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
	  //      printf("argv[%i]: %s\n",i,command->newargv[i]);
	  i++;
	}
      
      /* Checks if 'redo' syntax error is flagged and
	 jshell re-asks for a new correct line to be
	 inputted */
      
      struct Info *temp1;
      struct Info *infoTravel= head;
      
      if(redo)
	{
	  redo= 0;
	  
	  /* De-allocate Memory */
	  infoTravel= head;
	  while (infoTravel != NULL) {
	    temp1 = infoTravel;
	    infoTravel = infoTravel->next;
	    free(temp1);
	    free(temp1->newargv);
	  }
	  continue;
	}
      
      command->newargv[i]= NULL;
      
      /* Error Checking
	 Checks for empty newargv array (no commands and arguments) */
      
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
	  
	  infoTravel = head;
	  struct Info *temp2;
	  while (infoTravel!= NULL) {
	    temp2= infoTravel;
	    infoTravel = infoTravel->next;
	    free(temp2);
	    free(temp2->newargv);
	  }
	  continue;
	}
      
      /* FORK AND EXECUTE HERE */
      
      pid_t childpid;
      int pipein, pipeout, nextin;
      int pipefd[2];
      //int dummy;
      
      
      /* Loop through commands */
      // statTemp = statHead;
      struct Info *temp;
      for(temp= head; temp!= NULL; temp= temp->next)
	{
	  
	  /* If this is not the last command in the pipeline:
	     Call pipe to allcoate two file descriptors for the pipe
	     and save one as 'pipeout' and 'nextin' */
	  
	  if(temp->next!= NULL)
	    {
	      if(pipe(pipefd)< 0)
		{
		  perror("pipe");
		  exit(1);
		}
	      pipeout= pipefd[1];
	      nextin= pipefd[0];
	    }
	  
	  /* Call fork */
	  childpid= fork();
	  if(childpid< 0)
	    {
	      perror("fork");
	      exit(1);
	    }
	  
	  /* If this is the child process */
	  if(childpid== 0)
	    {
	      
	      /* If this is the first command
		 in the pipeline */
	      
	      if(temp== head)
		{
		  
		  /* If there is input re-direction */
		  
		  if(temp->input_file!= NULL)
		    {
		      close(0);
		      if(open(temp->input_file,O_RDWR,0666)< 0)
			{
			  perror("input file");
			  exit(1);
			}
		    }
		}
	      
	      /* Else not the first command */
	      
	      else
		{
		  if(close(0)< 0)
		    {
		      perror("close");
		  exit(1);
		    }
		  if(dup2(pipein,0)< 0)
		    {
		      perror("dup2");
		      exit(1);
		}
		  if(close(pipein)< 0)
		    {
		      perror("close");
		  exit(1);
		    }
		}
	      
	      /* If this is the last command in
		 the pipeline */
	      
	      if(temp->next== NULL)
		{
		  
		  /* If there is an output or append redirection */
		  
		  if(temp->output_file!= NULL)
		    {
		      close(1);
		      if(open(temp->output_file,O_RDWR|O_TRUNC|O_CREAT,0666)< 0)
			{
			  perror("cannot open output file");
			  exit(1);
			}
		    }
		  
		  if(temp->append_file!= NULL)
		    {
		      close(1);
		      int fd3= open(temp->append_file,O_RDWR|O_APPEND|O_CREAT,0666);
		      if(fd3< 0)
			{
			  perror("cannot open append output file");
			  exit(1);
			}
		    }
		}
	      
	      /* Else there is no output or append redirection */
	      
	      else
		{
		  if(close(nextin)< 0)
		    {
		      perror("close");
		      exit(1);
		    }
		  if(close(1)< 0)
		    {
		      perror("close");
		      exit(1);
		    }
		  if(dup2(pipeout,1)< 0)
		    {
		      perror("pipe");
		      exit(1);
		    }
		  if(close(pipeout)< 0)
		    {
		      perror("close");
		      exit(1);
		    }
		}
	      
	      /* Call execvp */
	      
	      execvp(temp->newargv[0],temp->newargv);
	      perror("execution failed -> check for invalid commands and arguments");
	      exit(1);
	    }
	  
	  /* -> else it is the parent */
	  
	  else
	    {
	      // Record childpid.
	      jobi = 0;
	      while (jobArr[jobi].pid != 0) 
		++jobi;
	      jobArr[jobi].pid = childpid;
	      if (background == 1) {
		jobArr[jobi].background = 1;
	      }
	      
	      
	      /*
		statTemp->pid = childpid;
		statTemp->next = (Status *)malloc(sizeof(Status));
		statTemp = statTemp->next;
		statTemp->next = NULL; // Not sure if this is necessary.     
	      */
	      
	      
	      /* If not the first process */
	      
	      if(temp!= head)
		{
		  if(close(pipein)< 0)
		    {
		      perror("close");
		      exit(1);
		    }
		}
	      
	      /* If not the last process */
	      
	      if(temp->next!= NULL)
		{
		  if(close(pipeout)< 0)
		    {
		      perror("close");
		      exit(1);
		    }
		  pipein= nextin;
		}
	    }
	}
      
      /* De-allocate Memory */
      infoTravel = head;
      while (infoTravel != NULL) {
	temp1 = infoTravel;
	infoTravel = infoTravel->next;
	free(temp1->newargv);
	free(temp1);
      }
      
      first = 0;
    }
  // free(statHead);
  
  return 0;
}
