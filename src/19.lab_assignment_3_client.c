/*
 * 19.lab_assignment_3_client.c
 *
 *  Created on: Oct 16, 2025
 *      Author: nishant
 *
 * Develop two processes that communicate using QNX message passing. The client shall send 3 types
 * of messages and the server shall perform different operation based on the message type and relpy
 * to the client
 */
#include "qnx_lab.h"
#include "19.msg_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>

typedef struct OperationPair {
  char* name;
  int msg_type;
} OperationPair;

static OperationPair operations[] = {
  {"add\n", MS_ADD_MSG_TYPE},
  {"sub\n", MS_SUB_MSG_TYPE},
  {"mult\n", MS_MULT_MSG_TYPE},
  {"div\n", MS_DIV_MSG_TYPE}
};
static const int operation_count = sizeof(operations) / sizeof(OperationPair);

int qnx_lab_assignment_3_client(int argc, char** argv, char** envp) {
  if (argc != 4) {
    printf("Usage: prog <server_pid> <channel_id>\n");
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

  char operation[10];
  char arguments[10];

  ms_request_t req;
  ms_response_t res;
  while (1) {
    printf("Enter opreation (add, sub, mult, div, exit): ");
    fgets(operation, 10, stdin);

    if (strcmp(operation, "exit\n") == 0) {
      break;
    }

    req.msg_type = 0;
    for (int i = 0; i < operation_count; ++i) {
      if (strcmp(operation, operations[i].name) == 0) {
        req.msg_type = operations[i].msg_type;
      }
    }

    if (req.msg_type == 0) {
      printf("Invalid opeartion\n");
      continue;
    }

    printf("Enter two numbers: ");
    fgets(arguments, 10, stdin);
    sscanf(arguments, "%d %d\n", &(req.a), &(req.b));

    int status = MsgSend(coid, &req, sizeof(req), &res, sizeof(res));
    if (status == -1) {
      fprintf(stderr, "MsgSend failed\n");
      exit(EXIT_FAILURE);
    }

    if (res.status == 0) {
      printf("Resutl: %lf\n", res.result);
    }
    else {
      printf("Server error: %s\n", res.error);
    }
  }

  return EXIT_SUCCESS;
}
