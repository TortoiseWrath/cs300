#define BUFFER_SIZE 5

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

int buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t full, empty;
int in_pos, out_pos;
int should_produce, should_consume;

#define MIN_PID 300
#define MAX_PID 50000

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

int insert_item(int item) {
	printf("Waiting for empty slot in buffer\n");
	int status = 0;

	sem_wait(&empty);
	if(!should_produce) status = -1;
	pthread_mutex_lock(&mutex);

	buffer[in_pos] = item;
	in_pos++;
	in_pos %= BUFFER_SIZE;

	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	if(!should_produce) sem_post(&empty); // Signal to next thread that they may continue as program is terminating

	return status;
}

int remove_item(int *item) {
	printf("Waiting for full slot in buffer\n");

	sem_wait(&full);
	pthread_mutex_lock(&mutex);

	*item = buffer[out_pos];
	out_pos++;
	out_pos %= BUFFER_SIZE;

	pthread_mutex_unlock(&mutex);
	sem_post(&empty);

	return 0;
}

void *producer() {
	// Request a pid
	int pid = allocate_pid();
	if(pid == 1) {
		fprintf(stderr, "Failed to allocate pid\n");
		return NULL;
	}

	while(should_produce) {
		int x = rand() % 4;
		int y = rand();
		printf("Producer thread %i is sleeping for %i second%c\n", pid, x, x==1?' ':'s');
		sleep(x);
		if(!should_produce) break;
		printf("Producer thread %i calling insert_item\n", pid);
		if(insert_item(y)) {
			// printf("Producer thread %i did not insert item %i as program is terminating\n", pid, y);
		}
		else {
			printf("Producer thread %i produced item %i\n", pid, y);
		}
	}

	release_pid(pid);
	return NULL;
}

void *consumer() {
	// Request a pid
	int pid = allocate_pid();
	if(pid == 1) {
		fprintf(stderr, "Failed to allocate pid\n");
		return NULL;
	}

	while(should_consume) {
		int x = rand() % 4;
		int y = rand();
		printf("Consumer thread %i is sleeping for %i second%c\n", pid, x, x==1?' ':'s');
		sleep(x);
		if(!should_consume) break;
		printf("Consumer thread %i callling remove_item\n", pid);
		remove_item(&x);
		printf("Consumer thread %i consumed item %i\n", pid, x);
	}

	release_pid(pid);
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Invalid arguments!\n");
		return -1;
	}

	int sleepfor, producers, consumers;
	sleepfor = atoi(argv[1]);
	producers = atoi(argv[2]);
	consumers = atoi(argv[3]);

	if(!(sleepfor && producers && consumers)) {
		printf("Invalid arguments!\n");
		return -1;
	}

	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	in_pos = out_pos = 0;
	srand(time(NULL));
	should_produce = should_consume = 1;

	printf("Buffer size is %i\n", BUFFER_SIZE);

	pthread_t producer_threads[producers];
	pthread_t consumer_threads[consumers];

	for(int i = 0; i < producers; i++) {
		pthread_create(&producer_threads[i], NULL, producer, NULL);
	}
	for(int i = 0; i < consumers; i++) {
		pthread_create(&consumer_threads[i], NULL, consumer, NULL);
	}

	printf("Main thread will sleep for %i second%c\n", sleepfor, sleepfor==1?' ':'s');
	sleep(sleepfor);

	printf("Terminating consumers\n");
	should_consume = 0; //tell consumers to terminate
	for(int i = 0; i < consumers; i++) {
		pthread_join(consumer_threads[i], NULL);
	}

	printf("Terminating producers\n");
	should_produce = 0; //tell producers to terminate
	for(int i = 0; i < producers; i++) {
		sem_post(&empty); //signal any blocking producer threads
		pthread_join(producer_threads[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

	return 0;
}
