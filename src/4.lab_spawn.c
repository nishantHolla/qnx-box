/*
 * 4.lab_spawn.c
 *
 *  Created on: Aug 14, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>


int qnx_lab_spawn(int argc, char** argv, char** envp) {
  (void) argc, (void) argv, (void) envp;

  pid_t pid;
  char* spawn_args[] = {"main", "hello", NULL};

  int ret = posix_spawn(&pid, "/tmp/main", NULL, NULL, &spawn_args[0], NULL);
  if (ret != 0) {
    fprintf(stderr, "Posix spawn failed.\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
