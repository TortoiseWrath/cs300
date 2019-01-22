#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

int avg, min, max;

void *average(void *nums_void) {
	int sum = 0;
	int *nums = (int *)nums_void;
	int count = *nums;
	for(int i = 1; i <= count; i++) {
		sum += *(nums + i);
	}
	avg = sum / count;
	return 0;
}

void *minimum(void *nums_void) {
	min = INT_MAX;
	int *nums = (int *)nums_void;
	int count = *nums;
	for(int i = 1; i <= count; i++) {
		if(*(nums + i) < min) {
			min = *(nums + i);
		}
	}
	return 0;
}

void *maximum(void *nums_void) {
	max = INT_MIN;
	int *nums = (int *)nums_void;
	int count = *nums;
	for(int i = 1; i <= count; i++) {
		if(*(nums + i) > max) {
			max = *(nums + i);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	if(argc<=1) {
		fprintf(stderr, "Require numbers\n");
		return 1;
	}
	int nums[argc]; //nums[0] is the count of numbers
	nums[0] = argc - 1;
	for(int i = 1; i < argc; i++) {
		for(int j = 0; argv[i][j]; j++) {
			if(!isdigit(argv[i][j]) && argv[i][j] != '-') {
				fprintf(stderr, "Invalid number: %s\n", argv[i]);
				return 1;
			}
		}
		nums[i] = atoi(argv[i]);
	}

	pthread_t average_thread;
	if(pthread_create(&average_thread, NULL, average, &nums)) {
		fprintf(stderr, "Error creating average thread\n");
		return 1;
	}
	pthread_t min_thread;
	if(pthread_create(&min_thread, NULL, minimum, &nums)) {
		fprintf(stderr, "Error creating minimum thread\n");
		return 1;
	}
	pthread_t max_thread;
	if(pthread_create(&max_thread, NULL, maximum, &nums)) {
		fprintf(stderr, "Error creating maximum thread\n");
		return 1;
	}

	int status = 0;

	if(pthread_join(average_thread, NULL)) {
		fprintf(stderr, "Error joining average thread\n");
		status = 1;
	}
	else {
		printf("The average value is %d\n", avg);
	}

	if(pthread_join(min_thread, NULL)) {
		fprintf(stderr, "Error joining minimum thread\n");
		status = 1;
	}
	else {
		printf("The minimum value is %d\n", min);
	}

	if(pthread_join(max_thread, NULL)) {
		fprintf(stderr, "Error joining maximum thread\n");
		status = 1;
	}
	else {
		printf("The maximum value is %d\n", max);
	}

	return status;
}
