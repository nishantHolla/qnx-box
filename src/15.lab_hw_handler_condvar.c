/*
 * 15.lab_hw_handler_condvar.c
 *
 *  Created on: Sep 11, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <pthread.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sched.h>

typedef struct QueueNode {
  struct QueueNode* next;
  int data;
} QueueNode_t;

static int* get_data_and_remove_from_queue(void);
static void write_to_hardware(int* data);
static void add_to_queue(int data);
static int add_element_to_queue(int data);
static int print_queue(void);

static int q_n_items;
static QueueNode_t* queue = NULL;
static int data_ready = 0;
static int running = 1;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void* hardware_handler(void* args);
static void* data_provider(void* args);

int qnx_lab_hw_handler_condvar(int argc, char** argv, char** envp) {
  pthread_t hw, dp;

  pthread_create(&hw, NULL, hardware_handler, NULL);
  pthread_create(&dp, NULL, data_provider, NULL);

  sleep(10);

  running = 0;
  pthread_join(hw, NULL);
  pthread_join(dp, NULL);

  return EXIT_SUCCESS;
}

static void* hardware_handler(void* args) {
  int* data = NULL;

  while (running) {
    pthread_mutex_lock(&mutex);

    while (!data_ready && running) {
      pthread_cond_wait(&cond, &mutex);
    }

    while ( (data = get_data_and_remove_from_queue()) != NULL) {
      pthread_mutex_unlock(&mutex);

      write_to_hardware(data);
      free(data);

      pthread_mutex_lock(&mutex);
    }

    data_ready = 0;
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

static void* data_provider(void* args) {
  srand(getpid());

  int r_sleep, n_loops;

  while (running) {
    n_loops = rand() % 10;

    for (int i = 0; i < n_loops; ++i) {
      add_to_queue(n_loops * 100 + i);
      sched_yield();
    }

    r_sleep = rand() % 15;
    usleep((r_sleep * 100 + 1) * 1000);
  }
  pthread_cond_signal(&cond);

  return NULL;
}

static void add_to_queue(int data) {
  pthread_mutex_lock(&mutex);

  add_element_to_queue(data);
  data_ready = 1;

  pthread_mutex_unlock(&mutex);
  pthread_cond_signal(&cond);
}

static int add_element_to_queue(int data) {
  QueueNode_t* new_node = NULL;
  QueueNode_t* end_of_queue = queue;

  printf("Adding data %d to the queue\n", data);

  new_node = (QueueNode_t*) malloc(sizeof(QueueNode_t));
  if (!new_node) {
    return 1;
  }
  else {
    new_node->next = NULL;
    new_node->data = data;
  }

  if (queue) {
    while (end_of_queue->next) {
      end_of_queue = end_of_queue->next;
    }
    end_of_queue->next = new_node;
  }
  else {
    queue = new_node;
  }

  ++q_n_items;
  return 0;
}

static int* get_data_and_remove_from_queue(void) {
  int* data = NULL;

  if (queue) {
    data = malloc(sizeof(int));

    QueueNode_t* current = queue;
    printf("Removing data %d from the queue\n", current->data);

    *data = current->data;
    queue = queue->next;

    free(current);
    --q_n_items;
    print_queue();
  }

  return data;
}

static void write_to_hardware(int* data) {
  char buff[255];

  int ret;
  ret = sprintf(buff, "Data written to hardware: %d, queue length: %d\n", *data, q_n_items);
  ret = write(STDOUT_FILENO, buff, ret);
  if (ret == -1) {
    fprintf(stderr, "Write failed\n");
    exit(EXIT_FAILURE);
  }
  usleep(2000);
}

static int print_queue(void) {
  printf("Queue: ");

  QueueNode_t* head = queue;
  while (head) {
    printf("%d ", head->data);
    head = head->next;
  }
  printf("\n");

  return 0;
}
