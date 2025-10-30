/*
 * 23.lab_shm_posix_consumer.c
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
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>


void* get_shared_memory_pointer(const char* name, unsigned int num_retries);

int qnx_lab_shm_posix_consumer(int argc, char** argv, char** envp) {
  int ret;
  uint64_t last_version = 0;
  char local_text[MAX_TEXT_LEN] = {0};

  if (argc != 3) {
    printf("Usasge: %s <shm_object_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  if (argv[2][0] != '/') {
    printf("shm_object_name should start with '/'");
    exit(EXIT_FAILURE);
  }

  const char* shm_object_name = argv[2];
  shm_obj_t* ptr = get_shared_memory_pointer(shm_object_name, 100);
  if (ptr == MAP_FAILED) {
    fprintf(stderr, "Unable to access object after %d tries\n", 100);
    exit(EXIT_FAILURE);
  }

  while (1) {
    ret = pthread_mutex_lock(&ptr->mutex);
    if (ret != EOK) {
      fprintf(stderr, "pthread_mutex_lock failed\n");
      exit(EXIT_FAILURE);
    }

    while (last_version == ptr->data) {
      ret = pthread_cond_wait(&ptr->cond, &ptr->mutex);
      if (ret != EOK) {
        fprintf(stderr, "pthread_cond_wait failed\n");
        exit(EXIT_FAILURE);
      }
    }

    last_version = ptr->data;
    strlcpy(local_text, ptr->text, sizeof(local_text));

    ret = pthread_mutex_unlock(&ptr->mutex);
    if (ret != EOK) {
      fprintf(stderr, "pthread_mutex_unlock failed\n");
      exit(EXIT_FAILURE);
    }

    printf("Data in shared memory is: %s\n", local_text);
  }

  return EXIT_SUCCESS;
}

void* get_shared_memory_pointer(const char* name, unsigned int num_retries) {
  int fd;
  for (unsigned int tries = 0;;) {
    fd = shm_open(name, O_RDWR, 0);
    if (fd != -1) {
      break;
    }

    ++tries;
    if (tries > num_retries) {
      fprintf(stderr, "shm_open failed\n");
      return MAP_FAILED;
    }

    sleep(1);
  }

  shm_obj_t* ptr;
  for (unsigned int tries = 0;;) {
    ptr = mmap(0, sizeof(shm_obj_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr != MAP_FAILED) {
      break;
    }

    ++tries;
    if (tries > num_retries) {
      fprintf(stderr, "mmap failed\n");
      return MAP_FAILED;
    }

    sleep(1);
  }

  close(fd);

  for (unsigned int tries = 0;;) {
    if (ptr->init_flag) {
      break;
    }

    ++tries;
    if (tries > num_retries) {
      fprintf(stderr, "init_falg not set\n");
      munmap(ptr, sizeof(shm_obj_t));
      return MAP_FAILED;
    }

    sleep(1);
  }

  return ptr;
}
