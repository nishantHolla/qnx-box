/*
 * 6.lab_pulse.c
 *
 *  Created on: Aug 21, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/procmgr.h>

int qnx_lab_pulse(int argc, char** argv, char** envp) {
  int chid, coid;
  int ret;
  struct _pulse pulse;
  struct sigevent ev;

  chid = ChannelCreate(_NTO_CHF_PRIVATE);
  if (chid == -1) {
    fprintf(stderr, "Failed to create channel.\n");
    exit(EXIT_FAILURE);
  }

  coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
  if (coid == -1) {
    fprintf(stderr, "Failed to connect to channel.\n");
    exit(EXIT_FAILURE);
  }

  SIGEV_PULSE_INIT(&ev, coid, 10, 1, 0);
  SIGEV_MAKE_UPDATEABLE(&ev);

  MsgRegisterEvent(&ev, SYSMGR_COID);
  procmgr_event_notify(PROCMGR_EVENT_PROCESS_DEATH, &ev);

  printf("Waiting for process death notification (Death Pulse) from the process manager.\n");

  while (1) {
    ret = MsgReceivePulse(chid, &pulse, sizeof(pulse), NULL);
    if (ret == -1) {
      fprintf(stderr, "Failed to receive pulse.\n");
      exit(EXIT_FAILURE);
    }

    if (pulse.code == 1) {
      printf("Process with pid: %d died\n", pulse.value.sival_int);
    }
  }
}
