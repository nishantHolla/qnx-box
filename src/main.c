#include "qnx_lab.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char** split_by_space(char* input);
static void prepend(char* s, const char* t);

int main(int argc, char** argv, char** envp) {
  char** args = NULL;
  int arg_count = 0;

  if (argc > 1) {
    args = (char **) malloc(sizeof(char*) * argc + 1);
    while (argv[arg_count]) {
      args[arg_count] = argv[arg_count];
      ++arg_count;
    }

    args[arg_count] = NULL;
  }
  else {
    char arg_line[256];

    printf("[MAIN]: Enter args: ");
    fgets(arg_line, sizeof(arg_line), stdin);
    arg_line[strcspn(arg_line, "\n")] = 0;
    prepend(arg_line, " ");
    prepend(arg_line, argv[0]);

    args = split_by_space(arg_line);
    while (args[arg_count]) {
      ++arg_count;
    }
  }

  printf("[MAIN]: Args(%d): ", arg_count);
  for (int i = 0; args[i]; ++i) {
    printf("%s ", args[i]);
  }
  printf("\n");

  if (arg_count < 2) {
    printf("[MAIN]: Usage: main [lab_name] [...arguments]\n");
    exit(EXIT_FAILURE);
  }

  if (strcmp(args[1], "help") == 0) {
    for (int i = 0; i < QNX_LAB_COUNT; ++i) {
      printf("%s\n", qnx_labs[i].name);
    }
    printf("\n");
  }
  else {
    for (int i = 0; i < QNX_LAB_COUNT; ++i) {
      if (strcmp(args[1], qnx_labs[i].name) == 0) {
        return qnx_labs[i].entry_point(arg_count, args, envp);
      }
    }

    printf("[MAIN]: Unrecognized entry point %s\n", args[1]);
  }

  free(args);
  return EXIT_FAILURE;
}

static char** split_by_space(char* input) {
  char** tokens = NULL;
  int count = 0;
  char* token = strtok(input, " \t\n");

  while (token) {
    tokens = realloc(tokens, sizeof(char*) * (count + 1));
    tokens[count] = strdup(token);
    ++count;
    token = strtok(NULL, " \t\n");
  }

  tokens = realloc(tokens, sizeof(char*) * (count + 1));
  tokens[count] = NULL;

  return tokens;
}

static void prepend(char* s, const char* t) {
  size_t len = strlen(t);
  memmove(s + len, s, strlen(s) + 1);
  memcpy(s, t, len);
}
