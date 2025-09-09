/*
 * 13.lab_condvar.c
 *
 *  Created on: Sep 9, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int ready = 0;

static void* worker(void* args);

int qnx_lab_condvar(int argc, char** argv, char** envp) {
  pthread_t tid;

  if (pthread_create(&tid, NULL, worker, NULL) != 0) {
    fprintf(stderr, "pthread_create failed\n");
    exit(EXIT_FAILURE);
  }

  printf("Main: Sleeping for 2 seconds...\n");
  sleep(2);

  pthread_mutex_lock(&mutex);

  ready = 1;
  printf("Main: Signaling condition variable...\n");
  pthread_cond_signal(&cond);

  pthread_mutex_unlock(&mutex);

  pthread_join(tid, NULL);
  printf("Main: Joined worker thread\n");

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);

  return EXIT_SUCCESS;
}

static void* worker(void* args) {
  printf("Worker: Started, waiting for condition...\n");
  pthread_mutex_lock(&mutex);

  while (ready == 0) {
    pthread_cond_wait(&cond, &mutex);
  }

  printf("Worker: Condition met. Proceeding...\n");
  pthread_mutex_unlock(&mutex);

  printf("Worker: Job done. Returning...\n");
  return NULL;
}
