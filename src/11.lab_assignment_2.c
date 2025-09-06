/*
 * 11.lab_assignment_2.c
 *
 *  Created on: Sep 4, 2025
 *      Author: nishant
 *
 * Write a program with 1 thread then with 4 to show the problem of global variable that the threads
 * compete for and fix it using mutex. Assume Round Robin scheduling.
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NUMBER_OF_THREADS 4

static volatile unsigned var1;
static volatile unsigned var2;
static volatile int done;
static pthread_mutex_t lock;

static void* thread_work(void *args);

int qnx_lab_assignment_2(int argc, char** argv, char** envp)  {
  int ret;

  pthread_t threadID[NUMBER_OF_THREADS];
  pthread_attr_t attrib;
  struct sched_param param;
  int policy;
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

  var1 = var2 = 0;
  printf("mutex_sync: starting; creating threads\n");

  if (use_mutex) {
    ret = pthread_mutex_init(&lock, NULL);
    if (ret != 0) {
      fprintf(stderr, "pthread_mutex_init failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  ret = pthread_getschedparam(pthread_self(), &policy, &param);
  if (ret != 0) {
    fprintf(stderr, "pthread_getschedparam failed. Error: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_attr_init(&attrib);
  if (ret != 0) {
    fprintf(stderr, "pthread_attr_init failed. Error: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_attr_setinheritsched(&attrib, PTHREAD_EXPLICIT_SCHED);
  if (ret != 0) {
    fprintf(stderr, "pthread_attr_setinheritsched failed. Error: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  ret = pthread_attr_setschedpolicy(&attrib, SCHED_RR);
  if (ret != 0) {
    fprintf(stderr, "pthread_attr_setschedpolicy failed. Error: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  param.sched_priority -= 2;
  ret = pthread_attr_setschedparam(&attrib, &param);
  if (ret != 0) {
    fprintf(stderr, "pthread_attr_setschedparam failed. Error: %s\n", strerror(ret));
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    ret = pthread_create(&threadID[i], &attrib, &thread_work, &use_mutex);
    if (ret != 0) {
      fprintf(stderr, "pthread_create failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  sleep(15);
  done = 1;

  for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
    ret = pthread_join(threadID[i], NULL);
    if (ret != 0) {
      fprintf(stderr, "pthread_join failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  if (use_mutex) {
    ret = pthread_mutex_destroy(&lock);
    if (ret != 0) {
      fprintf(stderr, "pthread_mutex_destroy failed. Error: %s\n", strerror(ret));
      exit(EXIT_FAILURE);
    }
  }

  printf("All done. var1 = %u, var2 = %u\n", var1, var2);
  return EXIT_SUCCESS;
}

void do_work(void) {
  static volatile unsigned var3 = 0;

  var3++;

  if (!(var3 % 100000)) {
    printf("thread %d did some work\n", (int)pthread_self());
  }
}

static void* thread_work(void *args) {
  int use_mutex = *(int *)args;
  int ret;

  while (!done) {

    if (use_mutex) {
      ret = pthread_mutex_lock(&lock);
      if (ret != 0) {
        fprintf(stderr, "pthread_mutex_lock failed. Error: %s", strerror(ret));
        exit(EXIT_FAILURE);
      }
    }

    if (var1 != var2) {
      printf("thread %d: var1 (%u) is not equal to var2 (%u)\n", (int)pthread_self(), var1, var2);
      var1 = var2;
    }

    do_work();

    var1 += 2;
    var1--;

    var2 += 2;
    var2--;

    if (use_mutex) {
      ret = pthread_mutex_unlock(&lock);
      if (ret != 0) {
        fprintf(stderr, "pthread_mutex_unlock failed. Error: %s", strerror(ret));
        exit(EXIT_FAILURE);
      }
    }
  }

  return NULL;
}
