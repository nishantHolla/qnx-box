#ifndef _19_MSG_DEF_H_
#define _19_MSG_DEF_H_

#include <sys/iomsg.h>
#include <stdint.h>

#define MAX_STRING_LEN 256
#define MS_ADD_MSG_TYPE 1
#define MS_SUB_MSG_TYPE 2
#define MS_MULT_MSG_TYPE 3
#define MS_DIV_MSG_TYPE 4

typedef struct {
  uint16_t msg_type;
  int a;
  int b;
} ms_request_t;

typedef struct {
  uint16_t msg_type;
  double result;
  int status;
  char error[MAX_STRING_LEN];
} ms_response_t;

#endif // !_19_MSG_DEF_H_
