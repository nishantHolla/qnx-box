/*
 * 18.lab_message_passing_client.c
 *
 *  Created on: Oct 14, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include "18.msg_def.h"


int qnx_lab_message_passing_client(int argc, char** argv, char** envp) {
  if (argc != 4) {
    printf("Usage: prog <server_pid> <channel_id>\n");
    exit(EXIT_FAILURE);
  }

  int server_pid = atoi(argv[2]);
  int server_chid = atoi(argv[3]);
  printf("Attempting to establish connection with server pid: %d, chid: %d\n", server_pid, server_chid);

  int coid = ConnectAttach(0, server_pid, server_chid, _NTO_SIDE_CHANNEL, 0);
  if (coid == -1) {
    fprintf(stderr, "ConnectAttach failed\n");
    exit(EXIT_FAILURE);
  }

  ss_request_t request;
  ss_response_t response;

  while (1) {
    char command[MAX_STRING_LEN];
    char string[MAX_STRING_LEN];

    printf("> ");
    fgets(command, MAX_STRING_LEN, stdin);

    if (strncmp(command, "exit\n", MAX_STRING_LEN) == 0) {
      break;
    }
    else if (strncmp(command, "help\n", MAX_STRING_LEN) == 0) {
      printf("Commands:\n\thelp\n\texit\n\tto_upper\n\tto_lower\n");
    }
    else if (strncmp(command, "to_upper\n", MAX_STRING_LEN) == 0) {
      fgets(string, MAX_STRING_LEN, stdin);
      request.msg_type = TO_UPPER_MSG_TYPE;
      strncpy(request.string, string, MAX_STRING_LEN);

      int status = MsgSend(coid, &request, sizeof(request), &response, sizeof(response));
      if (status == -1) {
        fprintf(stderr, "MsgSend failed\n");
        exit(EXIT_FAILURE);
      }

      if (response.status != 0) {
        fprintf(stderr, "Server error: %d\n", response.status);
      }
      else {
        printf("%s", response.result);
      }
    }
    else if (strncmp(command, "to_lower\n", MAX_STRING_LEN) == 0) {
      fgets(string, MAX_STRING_LEN, stdin);
      request.msg_type = TO_LOWER_MSG_TYPE;
      strncpy(request.string, string, MAX_STRING_LEN);

      int status = MsgSend(coid, &request, sizeof(request), &response, sizeof(response));
      if (status == -1) {
        fprintf(stderr, "MsgSend failed\n");
        exit(EXIT_FAILURE);
      }

      if (response.status != 0) {
        fprintf(stderr, "Server error: %d\n", response.status);
      }
      else {
        printf("%s", response.result);
      }
    }
    else {
      printf("Unknown command %s", command);
    }
  }

  return EXIT_SUCCESS;
}
