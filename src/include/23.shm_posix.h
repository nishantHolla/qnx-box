#ifndef _23_SHM_POSIX_H_
#define _23_SHM_POSIX_H_

#include <pthread.h>
#include <stdint.h>

#define MAX_TEXT_LEN 100

typedef struct {
  volatile unsigned init_flag;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  uint64_t data;
  char text[MAX_TEXT_LEN];
} shm_obj_t;

#endif // !_23_SHM_POSIX_H_
