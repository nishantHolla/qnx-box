/*
 * 25.lab_interrup_handler.c
 *
 *  Created on: Nov 4, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/neutrino.h>

#define INT_PRIORITY 15
#define INT_PULSE_CODE (_PULSE_CODE_MINAVAIL+0)

static void interrupt_thread(void);
static void interrupt_event(void);

int qnx_lab_interrupt_handler(int argc, char** argv, char** envp) {
  if (argc != 3) {
    printf("Usage: %s [thread|event]", argv[0]);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[2], "thread") == 0)  {
    interrupt_thread();
  }
  else if (strcmp(argv[2], "event") == 0) {
    interrupt_event();
  }

  return EXIT_SUCCESS;
}

void interrupt_thread(void) {
  int count = 0;

  printf("Starting interrupt_thread...\n");
  int id = InterruptAttachThread(1, 0); /// 1 is for keyboard
  if (id == -1) {
    fprintf(stderr, "InterruptAttachThread failed\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    if (InterruptWait(_NTO_INTR_WAIT_FLAGS_FAST, NULL) == -1) {
      fprintf(stderr, "InterruptWait failed\n");
      exit(EXIT_FAILURE);
    }

    if (InterruptUnmask(0, id) == -1) {
      fprintf(stderr, "InterruptUnmask failed\n");
    }

    printf("Got event and unblocked, count = %d\n", count);
    count++;
  }
}

void interrupt_event(void) {
  struct sigevent event;
  struct _pulse msg;

  int chid = ChannelCreate(_NTO_CHF_PRIVATE);
  int self_coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);

  SIGEV_PULSE_INIT(&event, self_coid, INT_PRIORITY, INT_PULSE_CODE, 0);

  int id = InterruptAttachEvent(1, &event, 0);
  int count = 0;

  printf("Starting interrupt_event...\n");
  while (1) {
    rcvid_t rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
    if (rcvid == 0) {
      InterruptUnmask(0, id);
      printf("Got event and unblocked, count = %d\n", count);
      count++;
    }
  }
}
