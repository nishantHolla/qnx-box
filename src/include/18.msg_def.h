#ifndef _18_MSG_DEF_H_
#define _18_MSG_DEF_H_

#include <sys/iomsg.h>
#include <stdint.h>

#define MAX_STRING_LEN 256
#define TO_UPPER_MSG_TYPE 1
#define TO_LOWER_MSG_TYPE 2

//layout of msgs should always defined by a struct, and ID'd by a msg type number as the first member

typedef struct {
  uint16_t msg_type;
  char string[MAX_STRING_LEN];
} ss_request_t;

typedef struct {
  uint16_t msg_type;
  char result[MAX_STRING_LEN];
  int status;
} ss_response_t;

#endif // !_18_MSG_DEF_H_
