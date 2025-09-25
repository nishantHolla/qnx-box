/*
 * 16.lab_produce_consume.c
 *
 *  Created on: Sep 11, 2025
 *      Author: nishant
 */
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define TO_PRODUCE 0
#define TO_PRODUCE_STR "TO_PRODUCE"

#define TO_CONSUME 1
#define TO_CONSUME_STR "TO_CONSUME"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static volatile int state = TO_PRODUCE;
static volatile int product = 0;
static int running = 1;

static void* producer(void* args);
static void* consumer(void* args);

int qnx_lab_produce_consume(int argc, char** argv, char** envp) {
  pthread_t pd, co;

  pthread_create(&co, NULL, consumer, NULL);
  pthread_create(&pd, NULL,producer, NULL);

  sleep(10);
  running = 0;

  pthread_join(co, NULL);
  pthread_join(pd, NULL);

  return EXIT_SUCCESS;
}

static void* producer(void* args) {
  while (running) {
    pthread_mutex_lock(&mutex);

    while (state != TO_PRODUCE && running) {
      pthread_cond_wait(&cond, &mutex);
    }

    state = TO_CONSUME;
    printf("produced %d, changed state to %s\n", ++product, TO_CONSUME_STR);

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    delay(100);
  }

  return NULL;
}

static void* consumer(void* args) {
  while (running) {
    pthread_mutex_lock(&mutex);

    while (state != TO_CONSUME && running) {
      pthread_cond_wait(&cond, &mutex);
    }

    state = TO_PRODUCE;
    printf("consumed %d, changed state to state %s\n\n", product, TO_PRODUCE_STR);

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    delay(100);
  }

  return NULL;
}
