/*
 * 3.lab_exec.c
 *
 *  Created on: Aug 14, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int qnx_lab_exec(int argc, char** argv, char** envp) {
	pid_t pid = fork();

	if (pid == 0) {
		execlp("ls", "ls", "-la", NULL);
	}
	else if (pid > 0) {
		waitpid(pid, NULL, 0);
		printf("Child terminated.\n");
	}
	else {
		fprintf(stderr, "Failed to fork\n");
	}

	return EXIT_SUCCESS;
}
