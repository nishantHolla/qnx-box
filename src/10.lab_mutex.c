/*
 * 10.lab_mutex.c
 *
 *  Created on: Sep 2, 2025
 *      Author: nishant
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int counter = 0;
pthread_mutex_t lock;

void* thread_func_sync(void* arg) {
  pthread_t id = pthread_self();

	for (int i = 0; i < 5; ++i) {
		pthread_mutex_lock(&lock);

		int local = counter;
		printf("Thread %d: counter = %d\n", (int)id, local);
		counter = local + 1;

		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

void* thread_func_no_sync(void* arg) {
  pthread_t id = pthread_self();

	for (int i = 0; i < 5; ++i) {
		int local = counter;
		printf("Thread %d: counter = %d\n", (int)id, local);
		counter = local + 1;
	}
	return NULL;
}

int qnx_lab_mutex(int argc, char** argv, char** envp) {
  (void) envp;

  bool use_mutex = true;

  if (argc == 3) {
    if (strcmp(argv[2], "sync") == 0) {
      use_mutex = true;
    }
    else if (strcmp(argv[2], "no_sync") == 0) {
      use_mutex = false;
    }
    else {
      fprintf(stderr, "Usage: a.out [sync|no_sync]\n");
      return 1;
    }
  }

  pthread_t t1, t2;

  if (use_mutex)  {
    printf("Using mutex\n");

    if (pthread_mutex_init(&lock, NULL) != 0) {
      fprintf(stderr, "Failed to init mutex\n");
      return 1;
    }

    pthread_create(&t1, NULL, thread_func_sync, NULL);
    pthread_create(&t2, NULL, thread_func_sync, NULL);
  }
  else {
    printf("Not using mutex\n");

    pthread_create(&t1, NULL, thread_func_no_sync, NULL);
    pthread_create(&t2, NULL, thread_func_no_sync, NULL);
  }

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("Final counter: %d\n", counter);

  if (use_mutex) {
    pthread_mutex_destroy(&lock);
  }
	return 0;
}

