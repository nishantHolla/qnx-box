#ifndef _21_MSG_DEF_H_
#define _21_MSG_DEF_H_

#include <sys/iomsg.h>

#define MAX_STRING_LEN 256
#define CKSUM_MSG_TYPE 1

typedef struct {
  uint16_t msg_type;
  char string[MAX_STRING_LEN];
} cs_request_t;

typedef struct {
  int status;
  int result;
  char error[MAX_STRING_LEN];
} cs_response_t;

#define SERVER_NAME "cksum_server"
#define CKSUM_PULSE_CODE (_PULSE_CODE_MINAVAIL + 3)

#endif // !_21_MSG_DEF_H_
