#ifndef _27_SHM_OP_H_
#define _27_SHM_OP_H_

#include <stdint.h>
#include <sys/iomsg.h>
#include <sys/mman.h>

#define GET_SHM_MSG_TYPE (_IO_MAX + 200)
#define CHANGED_SHM_MSG_TYPE (_IO_MAX + 201)
#define RELEASE_SHM_MSG_TYPE (_IO_MAX + 202)

#define SHM_SERVER_NAME "shm_server"

typedef struct {
  uint16_t type;
  unsigned int shared_mem_bytes;
} get_shm_msg_t;

typedef struct {
  shm_handle_t mem_handle;
} get_shm_res_t;

typedef struct {
  uint16_t type;
  unsigned int offset;
  unsigned int length;
} changed_shm_msg_t;

typedef struct {
  uint16_t type;
} release_shm_msg_t;

#endif // !_27_SHM_OP_H_
