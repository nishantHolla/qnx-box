/*
 * 7.lab_thread.c
 *
 *  Created on: Aug 26, 2025
 *      Author: nishant
 */
#include "qnx_lab.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static void* thread_work(void* args);

int qnx_lab_thread(int argc, char** argv, char** envp) {
	pthread_t thread;
	const char* message = "QNX Thread Example";
	printf("main: Creating thread...\n");

	if (pthread_create(&thread, NULL, thread_work, (void*)message) != 0) {
		fprintf(stderr, "main: Failed to create thread\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(thread, NULL) != 0) {
		fprintf(stderr, "main: Failed to join the thread");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

static void* thread_work(void* args) {
	printf("thraed: Hello from the thread! Argument = %s\n", (char*)args);
	return NULL;
}
