#define quantum 1

#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* BEGIN SLL */

/* A singly-linked list structure based on an implementation from CS 201 */

struct NODE {
	void *value;
	void *next;
};
typedef struct NODE node;

node *newNode(void *value, node *next) {
	node *n = malloc(sizeof(node));
	n->value = value;
	n->next = next;
	return n;
}

struct SLL {
	node *head;
	node *tail;
	int size;
	void (*free)(void *);
};
typedef struct SLL sll;

sll *newSLL(void (*f)(void *)) {
	sll *items = malloc(sizeof(sll));
	items->head = NULL;
	items->tail = NULL;
	items->size = 0;
	items->free = f;
	return items;
}

/* Returns a pointer to the node before the node at the given index.
   If the index is 0, returns a null pointer.
   Runs in constant time for nodes at a constant distance from the front. */
node *prevNode(sll *items, int index) {
	if(index < 0 || index > items->size) {
		printf("cannot get node before node %i from sll of size %i\n", index, items->size);
		exit(-1);
	}

	node *prev = NULL;

	if(index > 0) {
		node *n = items->head;
		int cur;
		for(cur = 0; cur < index; cur++) {
			// Step over the list until the node with the target index is reached.
			prev = n;
			n = n->next;
		}
	}

	return prev;
}

/* Returns a pointer to the node at the given index.
   Runs in constant time for nodes at the very back of the list.
   Runs in constant time for nodes at a constant distance from the front. */
node *getNode(sll *items, int index) {
	if(index < 0 || index >= items->size) {
		printf("cannot get node %i from sll of size %i\n", index, items->size);
		exit(-1);
	}

	// At the very back of the list?
	if(index == items->size - 1) {
		return items->tail;
	}

	if(index == 0) {
		return items->head;
	}

	/* Note that the node is the node after the previous node. */
	return prevNode(items, index)->next;
}

/* Runs in constant time for insertions at the very back of the list.
   Runs in constant time for insertions at a constant distance from the front. */
void insertSLL(sll *items, int index, void *value) {
	if(index < 0 || index > items->size) {
		printf("cannot insert at position %i in sll of size %i\n", index, items->size);
		exit(-1);
	}

	node *n = newNode(value, NULL);

	/* Insert only item? */
	if(items->size == 0) {
		items->head = n;
		items->tail = n;
	}

	/* Insert at the very back of the list? */
	else if(index == items->size) {
		items->tail->next = n;
		items->tail = n;
	}

	/* Insert somewhere else? */
	else {
		node *prev = prevNode(items, index);
		node *cur = prev ? prev->next : items->head;
		if (prev == NULL) { /* There is no previous node (current node is head) */
			items->head = n; /* Make the new head the head */
		}
		else {
			prev->next = n; /* The new node comes after the previous node. */
		}
		n->next = cur; /* The old node comes after the new node. */
	}

	items->size++;
	return;
}

/* Returns a pointer to the value of the removed node.
   Runs in constant time for removals at a constant distance from the front. */
void *removeSLL(sll *items, int index) {
	if(index < 0 || index >= items->size) {
		printf("cannot remove item %i from sll of size %i\n", index, items->size);
		exit(-1);
	}

	node *prev = prevNode(items, index);
	node *cur = prev ? prev->next : items->head;
	if (prev == NULL) {
		items->head = cur->next; /* Make the new head the head */
	}
	else {
		prev->next = cur->next;
	}

	if (index == items->size - 1) { /* If tail was removed */
		items->tail = prev; /* Tail is now the previous item. */
	}

	items->size--;

	void *val = cur->value;
	free(cur); /* free the node */
	return val;
}

/* Returns a pointer to the value of the node at the given index.
   Runs in constant time for retrievals at the very back of the list.
   Runs in constant time for retrievals at a constant distance from the front. */
void *getSLL(sll *items, int index) {
	if(index < 0 || index >= items->size) {
		printf("cannot get item %i from sll of size %i\n", index, items->size);
		exit(-1);
	}

	node *n = getNode(items, index);
	return n->value;
}

/* Returns a pointer to the value of the former node at the given index.
   If index == size, value is appended to the list and null pointer is returned.
   Runs in constant time for updates at the very back of the list.
   Runs in constant time for updates at a constant distance from the front. */
void *setSLL(sll *items, int index, void *value) {
	if(index < 0 || index > items->size) exit(-1);

	if(index == items->size) {
		insertSLL(items, items->size, value);
		return NULL;
	}

	node *n = getNode(items, index);
	void *oldval = n->value;
	n->value = value;
	return oldval;
}

/* Frees the values using the freeing function of the list.
   Frees the nodes.
   Frees the SLL. */
void freeSLL(sll *items) {
	node *n;
	node *n2 = items->head; /* Start at the head */
	while((n = n2)) {
		if(items->free != NULL) {
			(items->free)(n->value); /* free the value */
		}
		n2 = n->next; /* Save a pointer to the next node */
		free(n); /* Free the node */
	}

	free(items); /* Free the SLL */
	return;
}

/* END SLL */

/* BEGIN QUEUE */

/* Functions to use the SLL as a queue, based on an implementation from CS 201 */

void enqueue(sll *items, void *value) {
	insertSLL(items, items->size, value); /* insert at tail */
	return;
}

/* Returns a pointer to the value of the removed item. */
void *dequeue(sll *items) {
	return removeSLL(items, 0); /* remove the head */
}

/* Returns a pointer to the value of the item ready to come off the queue. */
void *peek(sll *items) {
	return getSLL(items, 0); /* peek at the head */
}

/* A struct to store processes */

struct PROCESS {
	int pid;
	int priority;
	int timeRemaining;
};
typedef struct PROCESS process;

process *newProcess(int priority, int time) {
	process *p = malloc(sizeof(process));
	p->pid = 0;
	p->priority = priority;
	p->timeRemaining = time;
	return p;
}

void freeProcess(void *p) {
	free((process *) p);
}



void terminateProcess(process *p) {
	kill(p->pid, SIGINT);
	waitpid(p->pid, NULL, WUNTRACED);
	freeProcess(p);
	return;
}

void suspendProcess(process *p) {
	kill(p->pid, SIGTSTP);
	waitpid(p->pid, NULL, WUNTRACED);
	return;
}

void startProcess(process *p) {
	int pid = fork();
	if(!pid) {
		char *args[2] = {"process", NULL};
		execvp("./process",args);
	}
	p->pid = pid;
	return;
}

void restartProcess(process *p) {
	kill(p->pid, SIGCONT);
	return;
}

int main(int argc, char *argv[]) {
	/* Queues for each priority level */
	sll *queues[4] = {newSLL(freeProcess), newSLL(freeProcess), newSLL(freeProcess), newSLL(freeProcess)};

	/* Open the input file */
	if(argc != 2) {
		printf("Require a text file as argument\n");
		exit(-1);
	}
	FILE *dispatchList;
	if(!(dispatchList = fopen(argv[1], "r"))) {
		printf("Failed to open %s for reading\n", argv[1]);
		exit(-1);
	}

	int maxTick = 0; /* the arrival time of the last process to arrive */
	process *runningProcess = NULL;

	/* Dispatcher loop */
	for(int tick = 0; tick == 0 || !sleep(quantum); tick++) {
		/* Read the file */
		int arrivalTime, priority, processorTime;
		while(fscanf(dispatchList, "%i, %i, %i\n", &arrivalTime, &priority, &processorTime) != EOF) {
			if(tick == 0 && arrivalTime > maxTick) {
				/* During the first tick, determine the arrival time of the last process. */
				maxTick = arrivalTime;
			}
			if(arrivalTime == tick) {
				/* Enqueue processes that have arrived */
				enqueue(queues[priority], newProcess(priority, processorTime));
			}
		}
		rewind(dispatchList); /* Return to start of the file for next tick */

		if(runningProcess != NULL) {
			runningProcess->timeRemaining--;
			if(runningProcess->timeRemaining) {
				if(runningProcess->priority == 0) {
					/* If a system process is not finished, let it continue until the next tick. */
					continue; /* Do not run any new processes this tick */
				}
				else {
					suspendProcess(runningProcess);
					if(runningProcess->priority < 3) {
						runningProcess->priority++; /* Move to lower priority level */
					}
					enqueue(queues[runningProcess->priority], runningProcess);
					runningProcess = NULL;
				}
			}
			else {
				terminateProcess(runningProcess);
				runningProcess = NULL;
			}
		}

		for(int i = 0; i < 4; i++) {
			if(queues[i]->size) {
				/* Run the next process */
				runningProcess = dequeue(queues[i]);
				if(runningProcess->pid) {
					restartProcess(runningProcess);
				}
				else {
					startProcess(runningProcess);
				}
				break; /* Done with this tick */
			}
		}

		if(runningProcess == NULL && tick > maxTick) {
			break;
			/* all processes have completed */
		}
	}

	/* Clean up */
	fclose(dispatchList);
	freeSLL(queues[0]);
	freeSLL(queues[1]);
	freeSLL(queues[2]);
	freeSLL(queues[3]);

	return 0;
}
