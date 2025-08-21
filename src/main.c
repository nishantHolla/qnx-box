#include "qnx_lab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv, char** envp) {
  const char* ep_name = (argc > 1) ? argv[1] : DEFAULT_ENTRY_POINT_NAME;

  for (int i = 0; i < QNX_LAB_COUNT; ++i) {
    if (strcmp(ep_name, qnx_labs[i].name) == 0) {
      return qnx_labs[i].entry_point(argc, argv, envp);
    }
  }

  printf("Unrecognized entry point %s\n", ep_name);
  return EXIT_FAILURE;
}
