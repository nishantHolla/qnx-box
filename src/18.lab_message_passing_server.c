/*
 * 18.lab_message_passing_server.c
 *
 *  Created on: Oct 14, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>
#include <string.h>
#include <ctype.h>

#include "18.msg_def.h"

static void string_to_upper(char* dest, char* src);
static void string_to_lower(char* dest, char* src);

int qnx_lab_message_passing_server(int argc, char** argv, char** envp) {
  int chid = ChannelCreate(0);
  if (chid == -1) {
    fprintf(stderr, "ChannelCreate failed\n");
    exit(EXIT_FAILURE);
  }

  int pid = getpid();
  printf("Server pid: %d, channel id: %d\n", pid, chid);

  ss_request_t request;
  ss_response_t response;

  while (1) {
    rcvid_t rcvid = MsgReceive(chid, &request, sizeof(request), NULL);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }

    if (request.msg_type == TO_UPPER_MSG_TYPE) {
      printf("Converting %s to upper\n", request.string);

      string_to_upper(response.result, request.string);
      response.status = 0;

      int status = MsgReply(rcvid, EOK, &response, sizeof(response));
      if (status == -1) {
        fprintf(stderr, "MsgReply failed\n");
        exit(EXIT_FAILURE);
      }
    }
    else if (request.msg_type == TO_LOWER_MSG_TYPE) {
      printf("Converting %s to lower\n", request.string);

      string_to_lower(response.result, request.string);
      response.status = 0;

      int status = MsgReply(rcvid, EOK, &response, sizeof(response));
      if (status == -1) {
        fprintf(stderr, "MsgReply failed\n");
        exit(EXIT_FAILURE);
      }
    }
    else {
      printf("Unknown message type %d\n", request.msg_type);
      if (MsgError(rcvid, ENOSYS) == -1) {
        fprintf(stderr, "MsgError\n");
      }
    }
  }

  return EXIT_SUCCESS;
}

static void string_to_upper(char* dest, char* src) {
  int i;

  for (i = 0; src[i] && i < MAX_STRING_LEN; ++i) {
    dest[i] = toupper(src[i]);
  }

  dest[i] = 0;
}

static void string_to_lower(char* dest, char* src) {
  int i;

  for (i = 0; src[i] && i < MAX_STRING_LEN; ++i) {
    dest[i] = tolower(src[i]);
  }

  dest[i] = 0;
}
