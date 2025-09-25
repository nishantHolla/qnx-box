/*
 * 17.lab_state_machine.c
 *
 *  Created on: Sep 25, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int state = 0;
int counter = 0;
int running = 1;

void* state_zero(void* args);
void* state_one(void* args);
void* state_two(void* args);
void* state_three(void* args);

int qnx_lab_state_machine(int argc, char** argv, char** envp) {
  pthread_t s0, s1, s2, s3;

  pthread_create(&s0, NULL, state_zero, NULL);
  pthread_create(&s1, NULL, state_one, NULL);
  pthread_create(&s2, NULL, state_two, NULL);
  pthread_create(&s3, NULL, state_three, NULL);

  sleep(15);
  running = 0;

  pthread_join(s0, NULL);
  pthread_join(s1, NULL);
  pthread_join(s2, NULL);
  pthread_join(s3, NULL);

  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond);

  return EXIT_SUCCESS;
}

void* state_zero(void* args) {
  while (running) {
    pthread_mutex_lock(&lock);

    while (state != 0) {
      pthread_cond_wait(&cond, &lock);
    }

    printf("s0: Reached state zero\n");
    state = 1;
    sleep(1);

    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond);
  }

  return NULL;
}

void* state_one(void* args) {
  while (running) {
    pthread_mutex_lock(&lock);

    while (state != 1) {
      pthread_cond_wait(&cond, &lock);
    }

    printf("s1: Reached state one. Incrementing counter\n");
    ++counter;
    state = (counter % 2 == 0) ? 2 : 3;

    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond);
  }

  return NULL;
}

void* state_two(void* args) {
  while (running) {
    pthread_mutex_lock(&lock);

    while (state != 2) {
      pthread_cond_wait(&cond, &lock);
    }

    printf("s2: Found counter to be even with value %d\n\n", counter);
    state = 0;

    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond);
  }

  return NULL;
}

void* state_three(void* args) {
  while (running) {
    pthread_mutex_lock(&lock);

    while (state != 3) {
      pthread_cond_wait(&cond, &lock);
    }

    printf("s3: Found counter to be odd with value %d\n\n", counter);
    state = 0;

    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond);
  }

  return NULL;
}
