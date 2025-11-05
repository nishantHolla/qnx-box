/*
 * 24.lab_shm_qnx_server.c
 *
 *  Created on: Nov 5, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "24.shm_qnx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

typedef union {
  uint16_t type;
  struct _pulse pulse;
  get_shm_msg_t get_shm;
  changed_shm_msg_t changed_shm;
} recv_buf_t;

static int create_shared_memory(unsigned int nbytes, int client_pid, void** ptr, shm_handle_t* handle);

int qnx_lab_shm_qnx_server(int argc, char** argv, char** envp) {
  if (argc < 3) {
    printf("Usage: %s <message_to_send>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char* resp = argv[2];
  unsigned int resp_len = strlen(resp);

  name_attach_t* att = name_attach(NULL, SHM_SERVER_NAME, 0);
  if (att == NULL) {
    fprintf(stderr, "name_attach failed\n");
    exit(EXIT_FAILURE);
  }

  recv_buf_t rbuf;
  struct _msg_info msg_info;
  int client_scoid = 0;
  unsigned int shm_memory_size;
  changed_shm_res_t changed_resp;
  get_shm_res_t get_resp;
  char* shm_ptr;

  while (1) {
    rcvid_t rcvid = MsgReceive(att->chid, &rbuf, sizeof(rbuf), &msg_info);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }
    else if (rcvid == 0) { /// pulse
      if (rbuf.pulse.code == _PULSE_CODE_DISCONNECT) {
        if (rbuf.pulse.scoid == client_scoid) {
          client_scoid = 0;
          munmap(shm_ptr, shm_memory_size);
        }
        ConnectDetach(rbuf.pulse.scoid);
      }
    }
    else { /// message
      switch (rbuf.type) {
        case GET_SHM_MSG_TYPE:
          shm_memory_size = rbuf.get_shm.shared_mem_bytes;
          if (shm_memory_size > 64 * 1024) {
            MsgError(rcvid, EINVAL);
            continue;
          }
          if (create_shared_memory(shm_memory_size, msg_info.pid, (void*)&shm_ptr, &get_resp.mem_handle) != 0) {
            MsgError(rcvid, errno);
            continue;
          }
          client_scoid = msg_info.scoid;

          if (MsgReply(rcvid, EOK, &get_resp, sizeof(get_resp)) == -1) {
            fprintf(stderr, "MsgReply failed\n");
            MsgError(rcvid, errno);
          }
          break;

        case CHANGED_SHM_MSG_TYPE:
          if (msg_info.scoid != client_scoid) {
            MsgError(rcvid, EPERM);
            continue;
          }

          const unsigned int offset = rbuf.changed_shm.offset;
          const unsigned int nbytes = rbuf.changed_shm.length;
          if (nbytes > shm_memory_size || offset > shm_memory_size || (nbytes + offset) > shm_memory_size) {
            MsgError(rcvid, EBADMSG);
            continue;
          }

          printf("Got from client: \n");
          write(STDOUT_FILENO, shm_ptr + offset, nbytes);
          write(STDOUT_FILENO, "\n", 1);

          changed_resp.offset = 4096 + 30;
          memcpy(shm_ptr + changed_resp.offset, resp, resp_len);
          changed_resp.length = resp_len;

          if (MsgReply(rcvid, EOK, &changed_resp, sizeof(changed_resp)) == -1) {
            fprintf(stderr, "MsgReply failed\n");
            MsgError(rcvid, errno);
          }
          break;

        case RELEASE_SHM_MSG_TYPE:
          if (msg_info.scoid != client_scoid) {
            MsgError(rcvid, EPERM);
            continue;
          }
          client_scoid = 0;
          munmap(shm_ptr, shm_memory_size);
          if (MsgReply(rcvid, EOK, NULL, 0) == -1) {
            fprintf(stderr, "MsgReply failed\n");
            MsgError(rcvid, errno);
          }
          break;

        default:
          if (MsgError(rcvid, ENOSYS) == -1) {
            fprintf(stderr, "MsgError\n");
          }
          break;
      }
    }
  }

  return EXIT_SUCCESS;
}

static int create_shared_memory(unsigned int nbytes, int client_pid, void** ptr, shm_handle_t* handle) {
  int fd = shm_open(SHM_ANON, O_RDWR | O_CREAT, 0600);
  if (fd == -1) {
    fprintf(stderr, "shm_open failed\n");
    return -1;
  }

  int ret = ftruncate(fd, nbytes);
  if (ret == -1) {
    fprintf(stderr, "ftruncate failed\n");
    return -1;
  }

  *ptr = mmap(NULL, nbytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (*ptr == MAP_FAILED) {
    fprintf(stderr, "mmap failed\n");
    close(fd);
    return -1;
  }

  printf("fd is %d, client_pid is %d\n", fd, client_pid);

  ret = shm_create_handle(fd, client_pid, O_RDWR, handle, 0);
  if (ret == -1) {
    fprintf(stderr, "shm_create_handle failed\n");
    close(fd);
    munmap(*ptr, nbytes);
    return -1;
  }

  close(fd);
  return 0;
}
