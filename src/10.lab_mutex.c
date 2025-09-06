/*
 * 10.lab_mutex.c
 *
 *  Created on: Sep 2, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

static int counter = 0;
static pthread_mutex_t lock;

static void* thread_work(void* arg);

int qnx_lab_mutex(int argc, char** argv, char** envp) {
  int use_mutex = 1;

  if (argc == 3) {
    if (strcmp(argv[2], "sync") == 0) {
      use_mutex = 1;
    }
    else if (strcmp(argv[2], "no_sync") == 0) {
      use_mutex = 0;
    }
    else {
      fprintf(stderr, "Usage: a.out [sync|no_sync]\n");
      return 1;
    }
  }

  pthread_t t1, t2;

  if (use_mutex)  {
    printf("Using mutex\n");

    int ret = pthread_mutex_init(&lock, NULL) != 0;
    if (ret != 0) {
      fprintf(stderr, "Failed to init mutex. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }
  else {
    printf("Not using mutex\n");
  }

  pthread_create(&t1, NULL, thread_work, &use_mutex);
  pthread_create(&t2, NULL, thread_work, &use_mutex);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("Final counter: %d\n", counter);

  if (use_mutex) {
    int ret = pthread_mutex_destroy(&lock);
    if (ret != 0) {
      fprintf(stderr, "pthread_mutex_destroy failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

	return EXIT_SUCCESS;
}

static void* thread_work(void* arg) {
  int use_mutex = *(int *) arg;
  int id = pthread_self();

	for (int i = 0; i < 5; ++i) {
    if (use_mutex) {
      int ret = pthread_mutex_lock(&lock);
      if (ret != 0) {
        fprintf(stderr, "pthread_mutex_lock failed. Error: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
      }
    }

		int local = counter;
		printf("Thread %d: counter = %d\n", (int)id, local);
		counter = local + 1;

    if (use_mutex) {
      int ret = pthread_mutex_unlock(&lock);
      if (ret != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed. Error: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
      }
    }
	}
	return NULL;
}
