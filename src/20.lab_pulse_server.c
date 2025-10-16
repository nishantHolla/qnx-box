/*
 * 20.lab_pulse_server.c
 *
 *  Created on: Oct 16, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "20.msg_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>

int qnx_lab_pulse_server(int argc, char** argv, char** envp) {
  int chid = ChannelCreate(0);
  if (chid == -1) {
    fprintf(stderr, "ChannelCreate failed\n");
    exit(EXIT_FAILURE);
  }

  int pid = getpid();
  printf("Server pid: %d, channel id: %d\n", pid, chid);

  struct _pulse req;

  while (1) {
    rcvid_t rcvid = MsgReceive(chid, &req, sizeof(req), NULL);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }
    else if (rcvid == 0) { /// pulse
      printf("Got a pulse with code %d and value %#lx\n", req.code, req.value.sival_long);
    }
    else { /// message
      if (MsgError(rcvid, ENOSYS) == -1) {
        fprintf(stderr, "MsgError\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  return EXIT_SUCCESS;
}
