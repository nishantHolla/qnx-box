/*
 * 8.lab_thread_term.c
 *
 *  Created on: Aug 28, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

static void* thread_func_cancel(void* args);
static void* thread_func_exit(void* args);
static void* thread_func_kill(void* args);

int qnx_lab_thread_term(int argc, char** argv, char** envp) {
  pthread_t tid_cancel, tid_exit, tid_kill;

  printf("main: Starting thread_func_cancel...\n");
  if (pthread_create(&tid_cancel, NULL, thread_func_cancel, NULL) != 0) {
    fprintf(stderr, "main: Failed to create thread_func_cancel\n");
    exit(EXIT_FAILURE);
  }
  printf("main: Created thread_func_cancel with tid: %d\n", (int)tid_cancel);

  printf("main: Starting thread_func_exit...\n");
  if (pthread_create(&tid_exit, NULL, thread_func_exit, NULL) != 0) {
    fprintf(stderr, "main: Failed to create thread_func_exit\n");
    exit(EXIT_FAILURE);
  }
  printf("main: Created thread_func_exit with tid: %d\n", (int)tid_exit);

  printf("main: Starting thread_func_kill...\n");
  if (pthread_create(&tid_kill, NULL, thread_func_kill, NULL) != 0) {
    fprintf(stderr, "main: Failed to create thread_func_kill\n");
    exit(EXIT_FAILURE);
  }
  printf("main: Create thread_func_kill with tid: %d\n", (int)tid_kill);

  sleep(3);

  printf("main: Cancelling thread_func_cancel...\n");
  pthread_cancel(tid_cancel);
  pthread_join(tid_cancel, NULL);

  printf("main: Joining thread_func_exit...\n");
  pthread_join(tid_exit, NULL);

  printf("main: Killing thread_func_kill...\n");
  pthread_kill(tid_kill, SIGKILL);

  sleep(2);
  printf("main: Completed termination program\n");
  fflush(stdout);

  return EXIT_SUCCESS;
}

static void* thread_func_cancel(void* args) {
  printf("thread_func_cancel: started\n");

  while (1) {
    printf("thread_func_cancel: running...\n");
    sleep(1);
    pthread_testcancel();
  }

  return NULL;
}

static void* thread_func_exit(void* args) {
  printf("thread_func_exit: started\n");

  sleep(2);
  printf("thread_func_exit: calling pthread_exit...\n");
  pthread_exit(NULL);

  return NULL;
}

static void* thread_func_kill(void* args) {
  printf("thread_func_kill: started\n");

  while (1) {
    printf("thread_func_kill: wailting to be killed...\n");
    sleep(1);
  }

  return NULL;
}
