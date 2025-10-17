/*
 * 21.lab_name_attach_client.c
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
#include <sys/iofunc.h>
#include <sys/dispatch.h>

int qnx_lab_name_attach_client(int argc, char** argv, char** envp) {
  if (argc != 3) {
    fprintf(stderr, "Usage: prog <string_to_send>\n");
    exit(EXIT_FAILURE);
  }

  int coid = name_open(SERVER_NAME, 0);
  if (coid == -1) {
    fprintf(stderr, "name_open failed\n");
    exit(EXIT_FAILURE);
  }

  if (MsgSendPulse(coid, -1, CKSUM_PULSE_CODE, 0xc0dedeed) == -1) {
    fprintf(stderr, "MsgSendPulse failed\n");
    exit(EXIT_FAILURE);
  }

  cs_request_t req;
  req.msg_type = CKSUM_MSG_TYPE;
  strlcpy(req.string, argv[2], sizeof(req.string));
  printf("Sending string: %s\n", req.string);

  cs_response_t res;
  int status = MsgSend(coid, &req, sizeof(req), &res, sizeof(res));
  if (status == -1) {
    fprintf(stderr, "MsgSend failed\n");
    exit(EXIT_FAILURE);
  }

  if (res.status == 0) {
    printf("Checksum of %s is %d\n", req.string, res.result);
  }
  else {
    printf("Server error: %d\n", res.status);
  }

  return EXIT_SUCCESS;
}
