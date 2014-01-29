#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  int sleeptime = 5;

  if (argc > 1)
    sleeptime = strtol(argv[1], NULL, 10);
  
  fprintf(stderr, "Sleeping for %i seconds.\n", sleeptime);
  sleep(sleeptime);
  fprintf(stderr, "Waking up.\n");

  return 0;
}
