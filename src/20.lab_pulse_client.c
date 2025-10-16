/*
 * 20.lab_pulse_client.c
 *
 *  Created on: Oct 16, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "20.msg_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>

int qnx_lab_pulse_client(int argc, char** argv, char** envp) {
  if (argc != 5) {
    printf("Usage: prog <server_pid> <channel_id> <message_to_send>\n");
    exit(EXIT_FAILURE);
  }

  int server_pid = atoi(argv[2]);
  int server_chid = atoi(argv[3]);

  printf("Attempting to establish connection with server pid: %d, channel id: %d\n", server_pid, server_chid);
  int coid = ConnectAttach(0, server_pid, server_chid, _NTO_SIDE_CHANNEL, 0);
  if (coid == -1) {
    fprintf(stderr, "ConnectAttach failed\n");
    exit(EXIT_FAILURE);
  }

  cs_request_t req;
  req.msg_type = CS_MSG_TYPE;
  strlcpy(req.string, argv[4], sizeof(req.string));
  printf("Sending string: %s\n", req.string);

  int status = MsgSendPulse(coid, -1, 3, 0xdeadc0de);
                     // id, priority, code, value
  if (status == -1) {
    fprintf(stderr, "MsgSendPulse failed\n");
    exit(EXIT_FAILURE);
  }

  cs_response_t res;
  status = MsgSend(coid, &req, sizeof(req), &res, sizeof(res));
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
