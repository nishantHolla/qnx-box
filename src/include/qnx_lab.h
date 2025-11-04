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
int qnx_lab_produce_consume(int argc, char** argv, char** envp);
int qnx_lab_state_machine(int argc, char** argv, char** envp);
int qnx_lab_message_passing_client(int argc, char** argv, char** envp);
int qnx_lab_message_passing_server(int argc, char** argv, char** envp);
int qnx_lab_assignment_3_client(int argc, char** argv, char** envp);
int qnx_lab_assignment_3_server(int argc, char** argv, char** envp);
int qnx_lab_pulse_client(int argc, char** argv, char** envp);
int qnx_lab_pulse_server(int argc, char** argv, char** envp);
int qnx_lab_name_attach_client(int argc, char** argv, char** envp);
int qnx_lab_name_attach_server(int argc, char** argv, char** envp);
int qnx_lab_events_client(int argc, char** argv, char** envp);
int qnx_lab_events_server(int argc, char** argv, char** envp);
int qnx_lab_shm_posix_producer(int argc, char** argv, char** envp);
int qnx_lab_shm_posix_consumer(int argc, char** argv, char** envp);
int qnx_lab_interrupt_handler(int argc, char** argv, char** envp);

static QNX_Lab qnx_labs[] = {
  { .name = "hello",                  .entry_point = qnx_hello                      },
  { .name = "fork",                   .entry_point = qnx_lab_fork                   },
  { .name = "exec",                   .entry_point = qnx_lab_exec                   },
  { .name = "spawn",                  .entry_point = qnx_lab_spawn                  },
  { .name = "zombie",                 .entry_point = qnx_lab_zombie                 },
  { .name = "pulse",                  .entry_point = qnx_lab_pulse                  },
  { .name = "thread",                 .entry_point = qnx_lab_thread                 },
  { .name = "thread_term",            .entry_point = qnx_lab_thread_term            },
  { .name = "assignment_1",           .entry_point = qnx_lab_assignment_1           },
  { .name = "mutex",                  .entry_point = qnx_lab_mutex                  },
  { .name = "assignment_2",           .entry_point = qnx_lab_assignment_2           },
  { .name = "orange_problem",         .entry_point = qnx_orange_problem             },
  { .name = "condvar",                .entry_point = qnx_lab_condvar                },
  { .name = "atomic",                 .entry_point = qnx_lab_atomic                 },
  { .name = "hw_handler_condvar",     .entry_point = qnx_lab_hw_handler_condvar     },
  { .name = "produce_consume",        .entry_point = qnx_lab_produce_consume        },
  { .name = "state_machine",          .entry_point = qnx_lab_state_machine          },
  { .name = "message_passing_client", .entry_point = qnx_lab_message_passing_client },
  { .name = "message_passing_server", .entry_point = qnx_lab_message_passing_server },
  { .name = "assignment_3_client",    .entry_point = qnx_lab_assignment_3_client    },
  { .name = "assignment_3_server",    .entry_point = qnx_lab_assignment_3_server    },
  { .name = "pulse_client",           .entry_point = qnx_lab_pulse_client           },
  { .name = "pulse_server",           .entry_point = qnx_lab_pulse_server           },
  { .name = "name_attach_client",     .entry_point = qnx_lab_name_attach_client     },
  { .name = "name_attach_server",     .entry_point = qnx_lab_name_attach_server     },
  { .name = "events_client",          .entry_point = qnx_lab_events_client          },
  { .name = "events_server",          .entry_point = qnx_lab_events_server          },
  { .name = "shm_posix_producer",     .entry_point = qnx_lab_shm_posix_producer     },
  { .name = "shm_posix_consumer",     .entry_point = qnx_lab_shm_posix_consumer     },
  { .name = "interrupt_handler",      .entry_point = qnx_lab_interrupt_handler      },
};
static const int QNX_LAB_COUNT = sizeof(qnx_labs) / sizeof(QNX_Lab);

#endif // !QNX_LAB_H_
