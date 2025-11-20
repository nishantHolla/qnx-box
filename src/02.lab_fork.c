/*
 * 2.lab_fork.c
 *
 *  Created on: Aug 14, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int qnx_lab_fork(int argc, char** argv, char** envp) {
	printf("Parent Process (Pid: %d)\n", getpid());
	pid_t pid = fork();

	if (pid == 0) {
		printf("Child Process (Pid: %d)\n", getpid());
	}
	else if (pid > 0) {
		waitpid(pid, NULL, 0);
	}
	else {
		fprintf(stderr, "Failed to fork\n");
	}

	return EXIT_SUCCESS;
}
