#include <stdbool.h>
#include <stdio.h>

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
