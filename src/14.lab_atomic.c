/*
 * 14.lab_atomic.c
 *
 *  Created on: Sep 9, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>

#define NUMBER_OF_THREADS 4
#define UPDATES 1000

static void* thread_work(void* args);
static atomic_int at_value = 0;
static int value = 0;

int qnx_lab_atomic(int argc, char** argv, char** envp) {
  pthread_t threads[NUMBER_OF_THREADS];
  int use_atomic = 1;

  if (argc == 3) {
    if (strcmp(argv[2], "atomic") == 0) {
      use_atomic = 1;
    }
    else if (strcmp(argv[2], "no_atomic") == 0) {
      use_atomic = 0;
    }
    else {
      fprintf(stderr, "Usage: a.out [atomic|no_atomic]\n");
      return 1;
    }
  }

  if (use_atomic) {
    printf("Starting value: %d\n", atomic_load(&at_value));
  }
  else {
    printf("Starting value: %d\n", value);
  }

  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    pthread_create(&threads[i], NULL, thread_work, &use_atomic);
  }

  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  int v = value;
  if (use_atomic) {
    v = atomic_load(&at_value);
    printf("Ending value: %d\n", v);
  }
  else {
    printf("Ending value: %d\n", value);
  }

  if (v != UPDATES * NUMBER_OF_THREADS) {
    printf("Error: Expected %d got %d\n", UPDATES * NUMBER_OF_THREADS, v);
  }

  return EXIT_SUCCESS;
}

static void* thread_work(void* args) {
  int use_atomic = *(int*)args;

  for (int i = 0; i < UPDATES; ++i) {
    if (use_atomic) {
      atomic_fetch_add(&at_value, 1);
      printf("(%d) Incrementing value\n", (int)pthread_self());
    }
    else {
      int old_value = value;
      printf("(%d) Incrementing value\n", (int)pthread_self());
      value = old_value + 1;
    }
  }
  return NULL;
}
