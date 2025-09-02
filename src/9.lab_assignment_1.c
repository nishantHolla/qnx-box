/*
 * 9.lab_assignment_1.c
 *
 *  Created on: Aug 28, 2025
 *      Author: nishant
 *
 * Implement a multi-threaded application using POSIX threads (pthread_create).
 * Each thread should process a different part of an array and the main thread
 * should wait for all threads to complete using pthread_join.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_COUNT 4
#define ARRAY_LENGTH 100

typedef struct ThreadArgs {
  int* array;
  int start_idx;
  int length;
  int thread_id;
} ThreadArgs_t;

void* double_array(void* args) {
  ThreadArgs_t* data = (ThreadArgs_t*) args;

  for (int i = data->start_idx, e = data->start_idx + data->length; i < e; ++i) {
    data->array[i] *= 2;
  }

  return NULL;
}

void print_array(int* array, int n) {
  for (int i = 0; i < n; ++i) {
    printf("%02d ", array[i]);
  }
  printf("\n");
}

int qnx_lab_assignment_1(void) {
  pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * THREAD_COUNT);
  ThreadArgs_t* thread_args = (ThreadArgs_t*) malloc(sizeof(ThreadArgs_t) * THREAD_COUNT);

  int* array = (int*) malloc(sizeof(int) * ARRAY_LENGTH);
  for (int i = 0; i < ARRAY_LENGTH; ++i) {
    array[i] = i;
  }
  int chunk_size = ARRAY_LENGTH / THREAD_COUNT;

  print_array(array, ARRAY_LENGTH);

  for (int i = 0; i < THREAD_COUNT; ++i) {
    thread_args[i].array = array;
    thread_args[i].start_idx = i * chunk_size;
    thread_args[i].length = chunk_size;
    thread_args[i].thread_id = i;

    if (pthread_create(&(threads[i]), NULL, double_array, (void*)&(thread_args[i])) != 0) {
      fprintf(stderr, "main: Failed to create thread\n");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("\n");
  print_array(array, ARRAY_LENGTH);

  free(threads);
  free(thread_args);
  free(array);
  return EXIT_SUCCESS;
}
