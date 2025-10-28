/*
 * 22.lab_events_client.c
 *
 *  Created on: Oct 17, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "22.events.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/siginfo.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <pthread.h>
#include <unistd.h>

#define MY_PULSE_CODE (_PULSE_CODE_MINAVAIL + 3)

union recv_msg {
  uint16_t type;
  struct _pulse pulse;
};

static int server_locate(void);

int qnx_lab_events_client(int argc, char** argv, char** envp) {
  int chid = ChannelCreate(_NTO_CHF_PRIVATE);
  if (chid == -1) {
    fprintf(stderr, "ChannelCreate failed\n");
    exit(EXIT_FAILURE);
  }

  int server_coid = server_locate();
  int self_coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
  if (self_coid == -1) {
    fprintf(stderr, "ConnectAttach failed\n");
    exit(EXIT_FAILURE);
  }

  notification_request_msg msg;
  msg.type = REQUEST_NOTIFICATIONS;

  struct sched_param sched_param;
  pthread_getschedparam(0, NULL, &sched_param);

  SIGEV_PULSE_INIT(&msg.ev, self_coid, sched_param.sched_priority, MY_PULSE_CODE, 0);

  SIGEV_MAKE_UPDATEABLE(&msg.ev);

  if (MsgRegisterEvent(&msg.ev, server_coid) == -1) {
    fprintf(stderr, "MsgRegisterEvent failed\n");
    exit(EXIT_FAILURE);
  }

  if (MsgSend(server_coid, &msg, sizeof(msg), NULL, 0) == -1) {
    fprintf(stderr, "MsgSend failed\n");
    exit(EXIT_FAILURE);
  }

  union recv_msg recv_buf;

  while (1) {
    rcvid_t rcvid = MsgReceive(chid, &recv_buf, sizeof(recv_buf), NULL);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }

    if (rcvid == 0) { /// pulse
      if (recv_buf.pulse.code == MY_PULSE_CODE) {
        printf("Got my pulse, value is %d\n", recv_buf.pulse.value.sival_int);
      }
      else {
        printf("Got unexpected pulse with code %d\n", recv_buf.pulse.code);
      }
    }
    else { /// message
      printf("Got unexpected message type %d\n", recv_buf.type);
      if (MsgError(rcvid, ENOSYS) == -1) {
        fprintf(stderr, "MsgError\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  return EXIT_SUCCESS;
}

static int server_locate(void) {
  int server_coid = -1;

  printf("Locating the server...");
  do {
    server_coid = name_open(RECV_NAME, 0);
  } while (server_coid == -1);

  return server_coid;
}
