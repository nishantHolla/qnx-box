/*
 * 19.lab_assignment_3_server.c
 *
 *  Created on: Oct 16, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "19.msg_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>

int qnx_lab_assignment_3_server(int argc, char** argv, char** envp) {
  int chid = ChannelCreate(0);
  if (chid == -1) {
    fprintf(stderr, "ChannelCreate failed\n");
    exit(EXIT_FAILURE);
  }

  int pid = getpid();
  printf("Server pid: %d, channel id: %d\n", pid, chid);

  ms_request_t req;
  ms_response_t res;
  while (1) {
    rcvid_t rcvid = MsgReceive(chid, &req, sizeof(req), NULL);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }

    printf("Got message type %d\n", req.msg_type);

    res.status = 0;
    switch (req.msg_type) {
      case MS_ADD_MSG_TYPE:
        res.result = req.a + req.b;
        break;

      case MS_SUB_MSG_TYPE:
        res.result = req.a - req.b;
        break;

      case MS_MULT_MSG_TYPE:
        res.result = req.a * req.b;
        break;

      case MS_DIV_MSG_TYPE:
        if (req.b == 0) {
          res.status = 2;
          strcpy(res.error, "Division by 0");
        }
        else {
          res.result = req.a / (double)req.b;
        }
        break;

      default:
        res.status = 1;
        strcpy(res.error, "Unknown operation");
        break;
    }

    int status = MsgReply(rcvid, EOK, &res, sizeof(res));
    if (status == -1) {
      fprintf(stderr, "MsgReply failed\n");
      exit(EXIT_FAILURE);
    }
  }

  return EXIT_SUCCESS;
}
