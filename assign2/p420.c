#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

int allocate_map(void);
int allocate_pid(void);
void release_pid(int);

void *sleeper() {
	// Request a pid
	int pid = allocate_pid();
	if(pid == 1) {
		fprintf(stderr, "Failed to allocate pid\n");
		return NULL;
	}
	printf("Allocated pid %i\n", pid);

	sleep(rand() % 4);

	release_pid(pid);
	printf("Released pid %i\n", pid);

	return NULL;
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	if(argc<=1) {
		fprintf(stderr, "Require argument\n");
		return 1;
	}
	int n = atoi(argv[1]);
	if(n<=0) {
		fprintf(stderr, "Invalid n=%d\n", n);
		return 1;
	}

	if(!allocate_map()) {
		fprintf(stderr, "failed to allocate map\n");
		return 1;
	}
	printf("Allocated map\n");
	pthread_t threads[n]; // To hold the thread references

	for(int i = 0; i < n; i++) {
		// Create n threads.
		if(pthread_create(&threads[i], NULL, sleeper, NULL)) {
			fprintf(stderr, "Failed to create thread %i\n", i);
		}
	}

	for(int i = 0; i < n; i++) {
		// Wait for all threads to terminate.
		if(pthread_join(threads[i], NULL)) {
			fprintf(stderr, "Failed to join thread %i\n", i);
		}
	}

	return 0;
}
