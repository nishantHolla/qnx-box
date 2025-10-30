/*
 * 23.lab_shm_posix_producer.c
 *
 *  Created on: Oct 30, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "23.shm_posix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>

void unlink_and_exit(const char* name);

int qnx_lab_shm_posix_producer(int argc, char** argv, char** envp) {
  int ret;

  if (argc != 3) {
    printf("Usage: %s <shm_object_name>", argv[0]);
    exit(EXIT_FAILURE);
  }
  if (argv[2][0] != '/') {
    printf("shm_object_name should start with '/'");
    exit(EXIT_FAILURE);
  }

  const char* shm_object_name = argv[2];
  printf("Creating shared memory object: %s\n", shm_object_name);

  int fd = shm_open(shm_object_name, O_RDWR | O_CREAT | O_EXCL, 0660);
  if (fd == -1) {
    fprintf(stderr, "shm_open failed\n");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(fd, sizeof(shm_obj_t)) == -1) {
    fprintf(stderr, "ftruncate failed\n");
    unlink_and_exit(shm_object_name);
  }

  shm_obj_t *ptr = mmap(0, sizeof(shm_obj_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    fprintf(stderr, "mmap failed\n");
    unlink_and_exit(shm_object_name);
  }

  close(fd);

  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t cond_attr;

  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
  ret = pthread_mutex_init(&ptr->mutex, &mutex_attr);
  if (ret != EOK) {
    fprintf(stderr, "pthread_mutex_init failed\n");
    unlink_and_exit(shm_object_name);
  }

  pthread_condattr_init(&cond_attr);
  pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
  ret = pthread_cond_init(&ptr->cond, &cond_attr);
  if (ret != EOK) {
    fprintf(stderr, "pthread_cond_init failed\n");
    unlink_and_exit(shm_object_name);
  }

  ptr->init_flag = 1;
  printf("Shared memory created and init_flat set to users know shared memory object is usable\n");

  while (1) {
    sleep(1);

    ret = pthread_mutex_lock(&ptr->mutex);
    if (ret != EOK) {
      fprintf(stderr, "pthread_mutex_lock failed\n");
      unlink_and_exit(shm_object_name);
    }

    ++ptr->data;
    snprintf(ptr->text, sizeof(ptr->text), "data update: %lu\n", ptr->data);

    ret = pthread_mutex_unlock(&ptr->mutex);
    if (ret != EOK) {
      fprintf(stderr, "pthread_mutex_unlock failed\n");
      unlink_and_exit(shm_object_name);
    }

    ret = pthread_cond_broadcast(&ptr->cond);
    if (ret != EOK) {
      fprintf(stderr, "pthread_cond_broadcast failed\n");
      unlink_and_exit(shm_object_name);
    }
  }

  if (munmap(ptr, sizeof(shm_obj_t)) == -1) {
    fprintf(stderr, "munmap failed\n");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(shm_object_name) == -1) {
    fprintf(stderr, "shm_unlink failed\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

void unlink_and_exit(const char* name) {
  (void) shm_unlink(name);
  exit(EXIT_FAILURE);
}

