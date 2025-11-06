/*
 * 29.lab_time.c
 *
 *  Created on: Nov 6, 2025
 *      Author: nishant
 *
 * Write a program that retrieves and displays the current system time using clock_gettime and allows
 * the user to set the system time using clock_settime by passing timer value from the command prompt
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/neutrino.h>

int qnx_lab_time(int argc, char** argv, char** envp) {
  char command[100];
  while (1) {
    printf("[set, get, exit] > ");
    fgets(command, sizeof(command), stdin);

    if (strcmp(command, "exit\n") == 0) {
      break;
    }
    else if (strcmp(command, "get\n") == 0) {
      struct timespec ts;

      if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        printf("Current real-time: %ld seconds, %ld nanoseconds\n", ts.tv_sec, ts.tv_nsec);
      }
      else {
        fprintf(stderr, "Failed to get real-time\n");
      }

      if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        printf("Current monotonic-time: %ld seconds, %ld nanoseconds\n", ts.tv_sec, ts.tv_nsec);
      }
      else {
        fprintf(stderr, "Failed to get monotonic-time\n");
      }
    }
    else if (strcmp(command, "set\n") == 0) {
      struct timespec ts;

      char line[100];
      printf("Enter seconds and nanoseconds: ");
      fgets(line, sizeof(line), stdin);
      sscanf(line, "%ld %ld", &ts.tv_sec, &ts.tv_nsec);

      if (clock_settime(CLOCK_REALTIME, &ts) == 0) {
        printf("Changed time\n");
      }
      else {
        fprintf(stderr, "Failed to change time\n");
      }
    }
  }

  return EXIT_SUCCESS;
}
