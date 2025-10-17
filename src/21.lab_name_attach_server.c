/*
 * 21.lab_name_attach_server.c
 *
 *  Created on: Oct 17, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "21.msg_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <unistd.h>

static int calculate_checksum(char* text);

typedef union {
  uint16_t type;
  cs_request_t req;
  struct _pulse pulse;
} recv_t;

int qnx_lab_name_attach_server(int argc, char** argv, char** envp) {
  name_attach_t* att = name_attach(NULL, SERVER_NAME, 0);
  if (!att) {
    fprintf(stderr, "name_attach failed\n");
    exit(EXIT_FAILURE);
  }

  int pid = getpid();
  printf("Server pid = %d, channel id = %d\n", pid, att->chid);

  recv_t rbuf;
  cs_response_t res;

  while (1) {
    rcvid_t rcvid = MsgReceive(att->chid, &rbuf, sizeof(rbuf), NULL);
    if (rcvid == -1) { /// invalid
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }
    else if (rcvid == 0)  { /// pulse
      printf("Pulse with code %d and value %#lx received\n", rbuf.pulse.code, rbuf.pulse.value.sival_long);

      switch (rbuf.pulse.code)  {
        case _PULSE_CODE_DISCONNECT:
          ConnectDetach(rbuf.pulse.scoid);
          break;

        case CKSUM_PULSE_CODE:
          printf("Expected code received: CKSUM_PULSE_CODE\n");
          break;

        default:
          printf("Unexpected pulse code\n");
          break;
      }
    }
    else { /// message
      printf("Message with type %d received\n", rbuf.type);

      switch (rbuf.type) {
        case CKSUM_MSG_TYPE:
          printf("Got a checksum message\n");
          res.result = calculate_checksum(rbuf.req.string);
          res.status = 0;
          int status = MsgReply(rcvid, EOK, &res, sizeof(res));
          if (status == -1) {
            fprintf(stderr, "MsgReply failed\n");
            exit(EXIT_FAILURE);
          }
          break;

        default:
          printf("Unknown message type\n");
          break;
      }
    }
  }

  return EXIT_SUCCESS;
}

static int calculate_checksum(char* text) {
  int sum = 0;

  for (int i = 0; text[i]; ++i) {
    sum += (int) text[i];
  }

  return sum;
}
