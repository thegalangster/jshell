#ifndef COMMAND_H 
#define COMMAND_H

typedef struct Command Command;

struct Command {
  char *input_file;
  char *output_file;
  char *append_file;
  int background;
  char **newargv;

  Command *next;
};

#endif
