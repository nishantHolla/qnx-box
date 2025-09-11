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
int qnx_lab_zombie(int argc, char** argv, char** envp);
int qnx_lab_pulse(int argc, char** argv, char** envp);
int qnx_lab_thread(int argc, char** argv, char** envp);
int qnx_lab_thread_term(int argc, char** argv, char** envp);
int qnx_lab_assignment_1(int argc, char** argv, char** envp);
int qnx_lab_mutex(int argc, char** argv, char** envp);
int qnx_lab_assignment_2(int argc, char** argv, char** envp);
int qnx_orange_problem(int argc, char** argv, char** envp);
int qnx_lab_condvar(int argc, char** argv, char** envp);
int qnx_lab_atomic(int argc, char** argv, char** envp);
int qnx_lab_hw_handler_condvar(int argc, char** argv, char** envp);

static QNX_Lab qnx_labs[] = {
  { .name = "hello",              .entry_point = qnx_hello                  },
  { .name = "fork",               .entry_point = qnx_lab_fork               },
  { .name = "exec",               .entry_point = qnx_lab_exec               },
  { .name = "spawn",              .entry_point = qnx_lab_spawn              },
  { .name = "zombie",             .entry_point = qnx_lab_zombie             },
  { .name = "pulse",              .entry_point = qnx_lab_pulse              },
  { .name = "thread",             .entry_point = qnx_lab_thread             },
  { .name = "thread_term",        .entry_point = qnx_lab_thread_term        },
  { .name = "assignment_1",       .entry_point = qnx_lab_assignment_1       },
  { .name = "mutex",              .entry_point = qnx_lab_mutex              },
  { .name = "assignment_2",       .entry_point = qnx_lab_assignment_2       },
  { .name = "orange_problem",     .entry_point = qnx_orange_problem         },
  { .name = "condvar",            .entry_point = qnx_lab_condvar            },
  { .name = "atomic",             .entry_point = qnx_lab_atomic             },
  { .name = "hw_handler_condvar", .entry_point = qnx_lab_hw_handler_condvar },
};
static const int QNX_LAB_COUNT = sizeof(qnx_labs) / sizeof(QNX_Lab);

#endif // !QNX_LAB_H_
