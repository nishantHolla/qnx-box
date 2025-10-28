/*
 * 22.lab_events_server.c
 *
 *  Created on: Oct 17, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"
#include "22.events.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

union recv_msgs {
  uint16_t type;
  struct _pulse pulse;
  struct notification_request_msg client_msg;
};

static pthread_mutex_t save_data_mutex;
static rcvid_t save_rcvid = 0;
static int notify_count = 0;
static int save_scoid = 0;
static struct sigevent save_event;

static void* notify_thread(void* args);

int qnx_lab_events_server(int argc, char** argv, char** envp) {
  name_attach_t* att = name_attach(NULL, RECV_NAME, 0);
  if (!att) {
    fprintf(stderr, "name_attach failed\n");
    exit(EXIT_FAILURE);
  }

  int status = pthread_mutex_init(&save_data_mutex, NULL);
  if (status != EOK) {
    fprintf(stderr, "pthread_mutex_init failed\n");
    exit(EXIT_FAILURE);
  }

  union recv_msgs recv_buf;
  struct _msg_info msg_info;

  pthread_t thread;
  status = pthread_create(&thread, NULL, notify_thread, NULL);
  if (status != EOK) {
    fprintf(stderr, "pthread_create failed\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    rcvid_t rcvid = MsgReceive(att->chid, &recv_buf, sizeof(recv_buf), &msg_info);
    if (rcvid == -1) {
      fprintf(stderr, "MsgReceive failed\n");
      exit(EXIT_FAILURE);
    }

    if (rcvid == 0) { /// pulse
      switch (recv_buf.pulse.code) {
        case _PULSE_CODE_DISCONNECT:
          status = pthread_mutex_lock(&save_data_mutex);
          if (status != EOK) {
            fprintf(stderr, "pthread_mutex_lock failed\n");
            exit(EXIT_FAILURE);
          }
          if (save_scoid == recv_buf.pulse.scoid) {
            save_scoid = 0;
            save_rcvid = 0;
            notify_count = 0;
          }
          status = pthread_mutex_unlock(&save_data_mutex);
          if (status != EOK) {
            fprintf(stderr, "pthread_mutex_unlock failed\n");
            exit(EXIT_FAILURE);
          }

          if (ConnectDetach(recv_buf.pulse.scoid) == -1) {
            fprintf(stderr, "ConnectDetach failed\n");
          }
          printf("Disconnected from client %#x\n", recv_buf.pulse.scoid);
          break;

        case _PULSE_CODE_UNBLOCK:
          printf("Go an unblock pulse, did you forget to reply to your client?\n");
          if (MsgError(recv_buf.pulse.value.sival_long, -1) == -1) {
            fprintf(stderr, "MsgError\n");
          }
          break;

        default:
          printf("Unexpected pulse code: %d\n", recv_buf.pulse.code);
          break;
      }
      continue;
    }

    switch (recv_buf.type) {
      case REQUEST_NOTIFICATIONS:
        if (MsgVerifyEvent(rcvid, &recv_buf.client_msg.ev) == -1) {
          fprintf(stderr, "MsgVerifyEvent");
          MsgError(rcvid, EINVAL);
          continue;
        }

        status = pthread_mutex_lock(&save_data_mutex);
        if (status != EOK) {
          fprintf(stderr, "pthread_mutex_lock failed\n");
          exit(EXIT_FAILURE);
        }

        save_rcvid = rcvid;
        save_event = recv_buf.client_msg.ev;

        save_scoid = msg_info.scoid;
        status = pthread_mutex_unlock(&save_data_mutex);
        if (status != EOK) {
          fprintf(stderr, "pthread_mutex_unlock failed\n");
          exit(EXIT_FAILURE);
        }

        if (MsgReply(rcvid, EOK, NULL, 0) == -1) {
          if (errno == ESRVRFAULT) {
            fprintf(stderr, "MsgReply fatal");
            exit(EXIT_FAILURE);
          }
          fprintf(stderr, "MsgReply failed\n");
        }

        printf("Got register message from client: %#lx\n", rcvid);
        break;

      default:
        printf("Unexpected message type: %d\n", recv_buf.type);
        if (MsgError(rcvid, ENOSYS) == -1) {
          fprintf(stderr, "MsgError");
        }
        break;
    }
  }

  pthread_join(thread, NULL);
  pthread_mutex_destroy(&save_data_mutex);

  return EXIT_SUCCESS;
}

static void* notify_thread(void* args) {
  int errornum;

  while (1)  {
    sleep(1);

    errornum = pthread_mutex_lock(&save_data_mutex);
    if (errornum != EOK) {
      fprintf(stderr, "pthread_mutex_lock failed\n");
      exit(EXIT_FAILURE);
    }

    if (save_rcvid) {
      printf("Deliver pulse to client %#lx\n", save_rcvid);
      if (save_event.sigev_notify & SIGEV_FLAG_UPDATEABLE) {
        save_event.sigev_value.sival_int = notify_count++;
      }

      if (MsgDeliverEvent(save_rcvid, &save_event) == -1) {
        const int save_errno = errno;
        fprintf(stderr, "MsgDeliverEvent failed\n");

        if (save_errno == EFAULT) {
          exit(EXIT_FAILURE);
        }
      }
    }

    errornum = pthread_mutex_unlock(&save_data_mutex);
    if (errornum != EOK) {
      fprintf(stderr, "pthread_mutex_unlock failed\n");
      exit(EXIT_FAILURE);
    }
  }

  return NULL;
}
