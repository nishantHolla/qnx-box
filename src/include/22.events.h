#ifndef _22_EVENTS_H_
#define _22_EVENTS_H_

#include <sys/siginfo.h>
#include <stdint.h>

#define RECV_NAME "MSG_RECEIVER"

typedef struct notification_request_msg {
  uint16_t type;
  struct sigevent ev;
} notification_request_msg;

#define REQUEST_NOTIFICATIONS (_IO_MAX+100)

#endif // !_22_EVENTS_H_
