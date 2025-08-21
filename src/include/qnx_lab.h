#ifndef QNX_LAB_H_
#define QNX_LAB_H_

#define MAX_LAB_NAME_LENGTH 100

typedef struct QNX_Lab {
  char name[MAX_LAB_NAME_LENGTH];
  int (*entry_point)(int argc, char** argv, char** envp);
} QNX_Lab;

int qnx_hello(int argc, char** argv, char** envp);
int qnx_lab_fork(int argc, char** argv, char** envp);
int qnx_lab_exec(int argc, char** argv, char** envp);
int qnx_lab_spawn(int argc, char** argv, char** envp);
int qnx_lab_pulse(int argc, char** argv, char** envp);

static QNX_Lab qnx_labs[] = {
  { .name = "hello", .entry_point = qnx_hello     },
  { .name = "fork",  .entry_point = qnx_lab_fork  },
  { .name = "exec",  .entry_point = qnx_lab_exec  },
  { .name = "spawn", .entry_point = qnx_lab_spawn },
  { .name = "pulse", .entry_point = qnx_lab_pulse }
};
static const int QNX_LAB_COUNT = sizeof(qnx_labs) / sizeof(QNX_Lab);
static const char DEFAULT_ENTRY_POINT_NAME[] = "pulse";

#endif // !QNX_LAB_H_
