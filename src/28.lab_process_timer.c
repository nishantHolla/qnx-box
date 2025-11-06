/*
 * 28.lab_process_timer.c
 *
 *  Created on: Nov 6, 2025
 *      Author: nishant
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

static int process_count = 0;
static pid_t process_pid;

static void child_task(void);
static void create_channel(int* chid, int* coid);
static void create_timer(timer_t* timerid, struct itimerspec* it, struct sigevent* event);

int qnx_lab_process_timer(int argc, char** argv, char** envp) {
  process_pid = fork();
  if (process_pid == 0) { /// child process
    child_task();
  }
  else if ( process_pid > 0 ) { /// parent process
    int chid, coid;
    create_channel(&chid, &coid);

    struct sigevent event;
    SIGEV_PULSE_INIT(&event, coid, 10, TIMER_PULSE_EVENT, 0);

    timer_t timerid;
    struct itimerspec it;
    create_timer(&timerid, &it, &event);

    message_t msg;
    while (1) {
      rcvid_t rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
      if (rcvid == -1) {
        fprintf(stderr, "MsgReceive failed\n");
        continue;
      }

      if (rcvid == 0) {
        switch (msg.pulse.code) {
          case TIMER_PULSE_EVENT:
            kill(process_pid, SIGTERM);
            printf("Killed process %d\n", process_count);

            ++process_count;
            process_pid = fork();
            if (process_pid == 0) {
              child_task();
            }
            else if (process_pid < 0) {
              printf("fork failed\n");
              exit(EXIT_FAILURE);
            }

            break;

          default:
            break;
        }
      }
    }
  }
  else {
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

static void child_task(void) {
  while (1) {
    printf("Child task %d running...\n", process_count);
    sleep(3);
  }
}

static void create_channel(int* chid, int* coid) {
  *chid = ChannelCreate(_NTO_CHF_PRIVATE);
  if (*chid == -1) {
    fprintf(stderr, "ChannelCreate failed\n");
    exit(EXIT_FAILURE);
  }

  *coid = ConnectAttach(0, 0, *chid, _NTO_SIDE_CHANNEL, 0);
  if (*coid == -1) {
    fprintf(stderr, "ConnectAttach failed\n");
    exit(EXIT_FAILURE);
  }
}

static void create_timer(timer_t* timerid, struct itimerspec* it, struct sigevent* event) {
  if (timer_create(CLOCK_MONOTONIC, event, timerid) == -1) {
    fprintf(stderr, "timer_create failed\n");
    exit(EXIT_FAILURE);
  }

  it->it_value.tv_sec = 10;
  it->it_value.tv_nsec = 0;
  it->it_interval.tv_sec = 10;
  it->it_interval.tv_nsec = 0;

  if (timer_settime(*timerid, 9, it, NULL) == -1) {
    fprintf(stderr, "timer_settime failed\n");
    exit(EXIT_FAILURE);
  }
}
