/*
 * 12.orange_problem.c
 *
 *  Created on: Sep 4, 2025
 *      Author: nishant
 *
 * Write a program to create a process with 4 threads that update the portion of array of size 1000
 * bytes by updating 250 bytes each. Make the main thread to join on the 4 threads and print the
 * completion. Use mutex to prevent data corruption while each thread is updating the array.
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define ARRAY_SIZE 1000
#define NUMBER_OF_THREADS 4
#define WORK_PER_THREAD (ARRAY_SIZE/NUMBER_OF_THREADS)
#define ALLOC_SIZE (WORK_PER_THREAD/10)

static pthread_mutex_t lock;
static int8_t array[ARRAY_SIZE] = {0};
static int allocated = 0;

static void print_array(int8_t* array, int size);
static void* thread_work(void* args);
static int8_t* buff_alloc(int alloc_size);

int qnx_orange_problem(int argc, char** argv, char** envp) {
  pthread_t threads[NUMBER_OF_THREADS];
  int ret;
  int use_mutex = 0;

  if (argc == 3) {
    if (strcmp(argv[2], "sync") == 0) {
      use_mutex = 1;
    }
    else if (strcmp(argv[2], "no_sync") == 0) {
      use_mutex = 0;
    }
    else {
      fprintf(stderr, "Usage: %s [sync|no_sync]\n", argv[0]);
      return 1;
    }
  }

  print_array(array, ARRAY_SIZE);

  if (use_mutex) {
    ret = pthread_mutex_init(&lock, NULL);
    if (ret != 0) {
      fprintf(stderr, "pthread_mutex_init failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    int ret = pthread_create(&threads[i], NULL, thread_work, &use_mutex);
    if (ret != 0) {
      fprintf(stderr, "pthread_create failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    int ret = pthread_join(threads[i], NULL);
    if (ret != 0) {
      fprintf(stderr, "pthread_join failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  print_array(array, ARRAY_SIZE);

  if (use_mutex) {
    ret = pthread_mutex_destroy(&lock);
    if (ret != 0) {
      fprintf(stderr, "pthread_mutex_destroy failed. Error: %s\n", strerror(ret));
    }
  }

  return EXIT_SUCCESS;
}

static void print_array(int8_t* array, int size) {
  for (int i = 0; i < size; ++i) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

static void* thread_work(void* args) {
  int use_mutex = *(int *)args;
  int work_done = 0;
  int ret;

  while (work_done < WORK_PER_THREAD) {

    if (use_mutex) {
      ret = pthread_mutex_lock(&lock);
      if (ret != 0) {
        fprintf(stderr, "pthread_mutex_lock failed. Error: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
      }
    }

    int8_t* allocation = buff_alloc(ALLOC_SIZE);

    if (use_mutex) {
      ret = pthread_mutex_unlock(&lock);
      if (ret != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed. Error: %s\n", strerror(ret));
        exit(EXIT_FAILURE);
      }
    }

    work_done += ALLOC_SIZE;

    if (!allocation) {
      fprintf(stderr, "buff_alloc failed\n");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ALLOC_SIZE; ++i) {
      // printf("Thread %d updated cell index %lu\n", (int)pthread_self(), allocation - array + i);
      allocation[i] += 1;
    }
  }

  return NULL;
}

static int8_t* buff_alloc(int alloc_size) {
  if (allocated >= ARRAY_SIZE || allocated + alloc_size > ARRAY_SIZE) {
    return NULL;
  }

  int8_t* allocation = &(array[allocated]);
  printf("Allocating chunk %3ld to thread %d\n", allocation - array, (int)pthread_self());
  allocated += alloc_size;

  return allocation;
}
