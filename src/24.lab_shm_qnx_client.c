/*
 * 24.lab_shm_qnx_client.c
 *
 *  Created on: Nov 5, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "24.shm_qnx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

int qnx_lab_shm_qnx_client(int argc, char** argv, char** envp) {
  if (argc != 3) {
    printf("Usage: %s <message_to_sedn>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int coid = name_open(SHM_SERVER_NAME, 0);
  if (coid == -1) {
    fprintf(stderr, "name_open failed\n");
    exit(EXIT_FAILURE);
  }

  get_shm_msg_t get_msg;
  get_msg.type = GET_SHM_MSG_TYPE;
  get_msg.shared_mem_bytes = 8192;

  get_shm_res_t get_res;

  if (MsgSend(coid, &get_msg, sizeof(get_msg), &get_res, sizeof(get_res)) == -1) {
    fprintf(stderr, "get_shm MsgSend failed\n");
    exit(EXIT_FAILURE);
  }

  int mem_fd = shm_open_handle(get_res.mem_handle, O_RDWR);
  if (mem_fd == -1) {
    fprintf(stderr, "shm_open_handle failed\n");
    exit(EXIT_FAILURE);
  }

  char* mem_ptr = mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
  if (mem_ptr == MAP_FAILED) {
    fprintf(stderr, "mmap failed\n");
    exit(EXIT_FAILURE);
  }

  close(mem_fd);
  char* msg = argv[2];
  unsigned int msg_len = strlen(msg);
  memcpy(mem_ptr + 20, msg, msg_len);

  changed_shm_msg_t changed_msg;
  changed_msg.type = CHANGED_SHM_MSG_TYPE;
  changed_msg.offset = 20;
  changed_msg.length = msg_len;

  changed_shm_res_t changed_res;

  if (MsgSend(coid, &changed_msg, sizeof(changed_msg), &changed_res, sizeof(changed_res)) == -1) {
    fprintf(stderr, "Changed shm MsgSend\n");
    exit(EXIT_FAILURE);
  }

  printf("Got from server: \n");
  write(STDOUT_FILENO, mem_ptr + changed_res.offset, changed_res.length);
  write(STDOUT_FILENO, "\n", 1);

  release_shm_msg_t release_msg;
  munmap(mem_ptr, 8192);
  release_msg.type = RELEASE_SHM_MSG_TYPE;
  if (MsgSend(coid, &release_msg, sizeof(release_msg), NULL, 0) == -1) {
    fprintf(stderr, "MsgSend failed\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

