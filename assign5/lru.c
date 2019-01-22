#define NUM_FRAMES 128

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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
	if(index < 0 || index >= items->size) return NULL;

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
	if(items->size == 0) return NULL;
	return getSLL(items, 0); /* peek at the head */
}

/* END QUEUE */

struct tlbEntryStruct {
	int page;
	int frame;
};
typedef struct tlbEntryStruct tlbEntry;

tlbEntry *newEntry(int page, int frame) {
	tlbEntry *e = malloc(sizeof(tlbEntry));
	e->page = page;
	e->frame = frame;
	return e;
}

int main(int argc, char *argv[]) {
	/* Open the input file and backing store */
	if(argc != 2) {
		printf("Require a text file as argument\n");
		exit(-1);
	}
	FILE *addresses, *backingStore;
	if(!(addresses = fopen(argv[1], "r"))) {
		printf("Failed to open %s for reading\n", argv[1]);
		exit(-1);
	}
	if(!(backingStore = fopen("BACKING_STORE.bin", "rb"))) {
		printf("Failed to open backing store for reading\n");
		exit(-1);
	}

	int faults = 0;
	int hits = 0;
	int accesses = 0;

	int pageTable[256] = {0};
	/* in page table values: lower 8 bits = frame; 9th bit (mask 0x100) = validity */
	char frames[NUM_FRAMES][256] = {0};
	int counters[NUM_FRAMES] = {0};
	sll *tlb = newSLL(free); /* a queue */

	for(int virtual = 0; fscanf(addresses, "%i", &virtual) == 1; accesses++) {
		int offset = virtual & 0xFF;
		int page = virtual >> 8 & 0xFF;

		int frame = 0;

		tlbEntry *entry = peek(tlb);
		for(int i = 1; entry != NULL; i++) {
			if(entry->page == page) {
				frame = entry->frame;
				break;
			}
			entry = getSLL(tlb, i);
		}

		if(frame) {
			/* TLB hit */
			hits++;
		}
		else {
			if(!(pageTable[page] & 0x100)) {
				/* Page fault */
				frame = faults++;
				if(frame >= NUM_FRAMES) {
					/* Replace a page */
					int LRUCounter = accesses;
					for(int i = 0; i < NUM_FRAMES; i++) {
						if(counters[i] < LRUCounter) {
							LRUCounter = counters[i];
							frame = i;
						}
					}
					int oldPage = -1;
					for(; oldPage < 256; oldPage++) {
						/* Find the page corresponding to the least recently used frame */
						if(pageTable[oldPage] & 0x100 && (pageTable[oldPage] & 0xFF) == frame) {
							break;
						}
					}
					pageTable[oldPage] = 0;
					/* There is no need to check the TLB since the relevant page will never be in the TLB if it is LRU and accesses > 16 which it must if frame >= NUM_FRAMES */
				}
				pageTable[page] = frame | 0x100;

				if(fseek(backingStore, page * 256, SEEK_SET)) {
					printf("Failed to seek to frame %i in backing store (%i: %s)\n", frame, errno, strerror(errno));
					//exit(-1);
				}
				if(fread(frames[frame], 256, 1, backingStore) != 1) {
					printf("Failed to read 256 bytes from the backing store (%i: %s)\n", errno, strerror(errno));
					//exit(-1);
				}
			}
			else {
				frame = pageTable[page] & 0xFF;
			}

			if(tlb->size == 16) free(dequeue(tlb));
			enqueue(tlb, newEntry(page, frame));
		}

		int physical = frame * 256 + offset;
		char value = frames[frame][offset];
		counters[frame] = accesses;

		printf("Virtual address: %i Physical address: %i Value: %i\n", virtual, physical, value);
	}

	/* Print statistics */
	printf("Number of Translated Addresses = %i\nPage Faults = %i\nPage Fault Rate = %.3f\nTLB Hits = %i\nTLB Hit Rate = %.3f\n",
		accesses,
		faults,
		(float) faults / accesses,
		hits,
		(float) hits / accesses
	);

	/* Clean up */
	fclose(addresses);
	fclose(backingStore);
	freeSLL(tlb);

	return 0;
}
