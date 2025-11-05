/*
 * 27.orange_problem_2_client.c
 *
 *  Created on: Nov 6, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "27.shm_op.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

#define SHM_SIZE 4096

int qnx_orange_problem_2_client(int argc, char** argv, char** envp) {
  int coid = name_open(SHM_SERVER_NAME, 0);
  if (coid == -1) {
    fprintf(stderr, "name_open failed\n");
    exit(EXIT_FAILURE);
  }

  get_shm_msg_t get_msg;
  get_msg.type = GET_SHM_MSG_TYPE;
  get_msg.shared_mem_bytes = SHM_SIZE;

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

  char* mem_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
  if (mem_ptr == MAP_FAILED) {
    fprintf(stderr, "mmap failed\n");
    exit(EXIT_FAILURE);
  }

  close(mem_fd);

  while (1) {
    printf("> ");
    char msg[SHM_SIZE] = {0};
    fgets(msg, sizeof(msg), stdin);
    unsigned int msg_len = strlen(msg);
    memcpy(mem_ptr, msg, msg_len);

    changed_shm_msg_t changed_msg;
    changed_msg.type = CHANGED_SHM_MSG_TYPE;
    changed_msg.offset = 0;
    changed_msg.length = msg_len;

    printf("Sending message %s", msg);
    if (MsgSend(coid, &changed_msg, sizeof(changed_msg), NULL, 0) == -1) {
      fprintf(stderr, "changed_shm MsgSend failed\n");
      exit(EXIT_FAILURE);
    }
  }

  release_shm_msg_t release_msg;
  munmap(mem_ptr, SHM_SIZE);
  release_msg.type = RELEASE_SHM_MSG_TYPE;
  if (MsgSend(coid, &release_msg, sizeof(release_msg), NULL, 0) == -1) {
    fprintf(stderr, "MsgSend failed\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
