#ifndef _20_MSG_DEF_H_
#define _20_MSG_DEF_H_

#include <sys/iomsg.h>
#include <stdint.h>

#define MAX_STRING_LEN 256
#define CS_MSG_TYPE 1

typedef struct {
  uint16_t msg_type;
  char string[MAX_STRING_LEN];
} cs_request_t;

typedef struct {
  int status;
  int result;
} cs_response_t;

#endif // !_20_MSG_DEF_H_
