#include "qnx_lab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* DEFAULT_ARGS[] = { "main", "mutex", "sync", NULL };

int main(int argc, char** argv, char** envp) {
  char** args = DEFAULT_ARGS;
  int arg_count = 0;

  while (DEFAULT_ARGS[arg_count]) {
    ++arg_count;
  }

  if (argc > 1) {
    arg_count = argc;
    args = argv;
  }

  for (int i = 0; i < QNX_LAB_COUNT; ++i) {
    if (strcmp(args[1], qnx_labs[i].name) == 0) {
      return qnx_labs[i].entry_point(arg_count, args, envp);
    }
  }

  printf("Unrecognized entry point %s\n", args[1]);
  return EXIT_FAILURE;
}
