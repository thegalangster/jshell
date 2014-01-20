#ifndef COMMAND_H 
#define COMMAND_H

typedef struct Command Command;

struct Command {
  char *input_file; // Will these file vars be set to NULL if they're not used?
  char *output_file; // I don't think they will implicitly, at least not reliably.
  char *append_file;
  int background;
  char **newargv;

  Command *next;
};

// Can we use this to contain the linked list of commands?
typedef struct {
  Command* head;
  int length;
} Pipeline;

// length needs to be initialized to 0 when a new pipeline is created,
// so you could use something like this
Pipeline* newPipeline() {
  Pipeline* newpl = (Pipeline *)malloc(sizeof(Pipeline));
  newpl->length = 0;
  newpl->head = (Command *)malloc(sizeof(Command));
  return newpl;
}
// and this to free
void deletePipeline(Pipeline *pl) {
  free(pl->head);
  free(pl);
}
// NOTE: NEITHER OF THESE ARE TESTED


#endif
