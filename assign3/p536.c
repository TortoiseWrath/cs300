#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>


#define MIN_PID 300
#define MAX_PID 5000

bool map[MAX_PID - MIN_PID + 1];

int allocate_map(void) {
	for(int i = 0; i < MAX_PID - MIN_PID + 1; i++) {
		if((map[i] = false)) {
			fprintf(stderr, "Failed to set map[%d] = false; map[%d] == %d\n", i, i, map[i]);
			return 0;
		}
	}
	return 1;
}

int allocate_pid(void) {
	int i = 0;
	while(map[i]) {
		i++;
		if(i > MAX_PID - MIN_PID) return 1;
	}
	map[i] = true;
	return i + MIN_PID;
}

void release_pid(int pid) {
	map[pid - MIN_PID] = false;
}


int counter;
pthread_mutex_t mutex;

void *sleeper() {
	// Request a pid
	int pid = allocate_pid();
	if(pid == 1) {
		fprintf(stderr, "Failed to allocate pid\n");
		return NULL;
	}

	int x = rand() % 4;
	sleep(rand() % 4);

	pthread_mutex_lock(&mutex);

	counter++;
	printf("Sleeping time: %i secs ; Thread Id = %i; Counter Value = %i.\n", x, pid, counter);

	pthread_mutex_unlock(&mutex);

	release_pid(pid);

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
	pthread_t threads[n]; // To hold the thread references

	pthread_mutex_init(&mutex, NULL);

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

	pthread_mutex_destroy(&mutex);

	return 0;
}
