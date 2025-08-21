/*
 * 5.lab_zombie.c
 *
 *  Created on: Aug 21, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <errno.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int qnx_lab_zombie(int argc, char **argv, char **envp) {
  (void) argc, (void) argv;

  pid_t pid;
  int child_status;
  int ret;

  printf("Parent pid is %d\n", getpid());

  char *child_argv[] = {"sleep", "5", NULL};
  ret = posix_spawn(&pid, "/system/bin/sleep", NULL, NULL, child_argv, envp);
  if (ret != 0) {
    fprintf(stderr, "posix_spawn failed (%d) %s\n", ret, strerror(ret));
    exit(EXIT_FAILURE);
  }
  sleep(5);

  printf("child pid is %d\n", pid);
  printf("View the process list in the IDE or at the command line.\n");
  printf("In the IDE Target Navigator menu try grpup->by PID family.\n");
  printf("With pidin, try 'pidin family' to get parent/child information.\n");

  printf("Child has died, pidin should now show it as zombie\n");
  sleep(30);

  pid = wait(&child_status);
  if (pid == -1) {
    perror("Wait");
    exit(EXIT_FAILURE);
  }
  printf("child process: %d, died with status %x\n", pid, child_status);
  printf("Zombie is now gone as we've waited on the child process.\n");
  return 0;
}
