/*
 * 26.lab_timer.c
 *
 *  Created on: Nov 4, 2025
 *      Author: nishant
 *
 * We want to have the server receive maintenance timer message every 1.2 seconds, so that it can go
 * out and perform housekeeping duties / integrity checks etc.
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/neutrino.h>

#define TIMER_PULSE_EVENT (_PULSE_CODE_MINAVAIL + 7)

typedef union {
  struct _pulse pulse;
} message_t;

int qnx_lab_timer(int argc, char** argv, char** envp) {
  struct sigevent event;
  message_t msg;

  int chid = ChannelCreate(_NTO_CHF_PRIVATE);
  if (chid == -1) {
    fprintf(stderr, "ChannelCreate failed\n");
    exit(EXIT_FAILURE);
  }

  int coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
  if (coid == -1) {
    fprintf(stderr, "ConnectAttach failed\n");
    exit(EXIT_FAILURE);
  }
  SIGEV_PULSE_INIT(&event, coid, 10, TIMER_PULSE_EVENT, 0);

  timer_t timerid;
  struct itimerspec it;

  if (timer_create(CLOCK_MONOTONIC, &event, &timerid) == -1) {
    fprintf(stderr, "timer_create failed\n");
    exit(EXIT_FAILURE);
  }

  it.it_value.tv_sec = 5;
  it.it_value.tv_nsec = 0;
  it.it_interval.tv_sec = 1;
  it.it_interval.tv_nsec = 500 * 1000 * 1000; /// 5^8 ns = 0.5 s
  if (timer_settime(timerid, 0, &it, NULL) == -1) {
    fprintf(stderr, "timer_settime failed\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    rcvid_t rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      continue;
    }

    if (rcvid == 0) {
      switch (msg.pulse.code) {
        case TIMER_PULSE_EVENT:
          printf("got our pulse, the timer must have expired\n");
          break;

        default:
          printf("unexpected pulse code: %d\n", msg.pulse.code);
          break;
      }
    }
  }

  return EXIT_SUCCESS;
}
