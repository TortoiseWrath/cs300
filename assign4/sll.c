#include "sll.h"
#include <stdlib.h>
#include <assert.h>

typedef struct sllnode SLL_NODE;

// Functions to get nodes (not their values) from the list
static SLL_NODE *getPrevNodeSLL(SLL *items, int index);
static SLL_NODE *getNodeSLL(SLL *items, int index);

// Storage of nodes
static SLL_NODE *newSLL_NODE(void *value, SLL_NODE *next);
static void *getSLL_NODEvalue(SLL_NODE *n);
static SLL_NODE *getSLL_NODEnext(SLL_NODE *n);
static void setSLL_NODEvalue(SLL_NODE *n, void *value);
static void setSLL_NODEnext(SLL_NODE *n, SLL_NODE *next);

struct sll {
	SLL_NODE *head;
	SLL_NODE *tail;
	int size;
	void (*display)(void *, FILE *);
	void (*free)(void *);
};

// newSLL implementation taken from the assignment.
// Modified for conventions consistent with remainder of code.
// d is the display function
// f is the freeing function
SLL *newSLL(void (*d)(void *, FILE *), void (*f)(void *)) {
	SLL *items = malloc(sizeof(SLL));
	assert(items != NULL); // The memory allocated shall not be null.
	items->head = NULL;
	items->tail = NULL;
	items->size = 0;
	items->display = d;
	items->free = f;
	return items;
}


// Runs in constant time for insertions at the very back of the list.
// Runs in constant time for insertions at a constant distance from the front.
void insertSLL(SLL *items, int index, void *value) {
	assert(index >= 0);
	assert(index <= items->size);

	SLL_NODE *n = newSLL_NODE(value, NULL);

	// Insert only item?
	if(items->size == 0) {
		items->head = n;
		items->tail = n;
	}

	// Insert at the very back of the list?
	else if(index == items->size) {
		setSLL_NODEnext(items->tail, n);
		items->tail = n;
	}

	// Insert somewhere else?
	else {
		SLL_NODE *prev = getPrevNodeSLL(items, index);
		SLL_NODE *cur = prev ? getSLL_NODEnext(prev) : items-> head;
		if (prev == NULL) { // There is no previous node (current node is head)
			items->head = n; // Make the new head the head
		}
		else {
			setSLL_NODEnext(prev, n); // The new node comes after the previous node.
		}
		setSLL_NODEnext(n, cur); // The old node comes after the new node.
	}

	items->size++;
	return;
}

// Returns a pointer to the value of the removed node.
// Runs in constant time for removals at a constant distance from the front.
void *removeSLL(SLL *items, int index) {
	assert(items->size >= 0);
	assert(index >= 0);
	assert(index < items->size);

	SLL_NODE *prev = getPrevNodeSLL(items, index);
	SLL_NODE *cur = prev ? getSLL_NODEnext(prev) : items->head;
	if (prev == NULL) {
		items->head = getSLL_NODEnext(cur); // Make the new head the head
	}
	else {
		setSLL_NODEnext(prev, getSLL_NODEnext(cur));
	}

	if (index == items->size - 1) { // If tail was removed
		items->tail = prev; // Tail is now the previous item.
	}

	items->size--;

	void *val = getSLL_NODEvalue(cur);
	free(cur); //free the node
	return val;

}

// Moves all items in the donor list to the end of the recipient list.
// Runs in constant time.
// Does not check whether any nodes are identical between the two lists.
// If two nodes are identical (same address) there will be problems.
void unionSLL(SLL *recipient, SLL *donor) {
	if(donor->size == 0) { //empty donor
		return; //do nothing
	}
	if(recipient->size == 0) {
		recipient->head = donor->head;
	}
	else {
		setSLL_NODEnext(recipient->tail, donor->head); // Transplant the head.
	}
	recipient->tail = donor->tail;
	recipient->size += donor->size; // Sum the sizes.

	// Clear the donor.
	donor->head = NULL;
	donor->tail = NULL;
	donor->size = 0;

	return;
}

// Returns a pointer to the value of the node at the given index.
// Runs in constant time for retrievals at the very back of the list.
// Runs in constant time for retrievals at a constant distance from the front.
void *getSLL(SLL *items, int index) {
	assert(index >= 0);
	assert(index < items->size);

	SLL_NODE *n = getNodeSLL(items, index);
	return getSLL_NODEvalue(n);
}

// Returns a pointer to the value of the former node at the given index.
// If index == size, value is appended to the list and null pointer is returned.
// Runs in constant time for updates at the very back of the list.
// Runs in constant time for updates at a constant distance from the front.
void *setSLL(SLL *items, int index, void *value) {
	assert(index >= 0);
	assert(index <= items->size);

	if (index == items->size) {
		insertSLL(items, items->size, value);
		return NULL;
	}

	SLL_NODE *n = getNodeSLL(items, index);
	void *oldval = getSLL_NODEvalue(n);
	setSLL_NODEvalue(n, value);
	return oldval;
}

int sizeSLL(SLL *items) {
	return items->size;
}

// Outputs in the format: {5,6,2,9,1}
// Uses the display function of the list.
void displaySLL(SLL *items, FILE *fp) {
	assert(items->display != NULL);
	fputc('{', fp);

	SLL_NODE *n = items->head; // Start at the head
	if(n != NULL) { // Skip if there is no head (empty)
		int ishead = 1;
		do {
			if(!ishead) { // Put commas before non-head values
				fputc(',', fp);
			}
			ishead = 0; // All values after head will be non-head
			(items->display)(getSLL_NODEvalue(n), fp);
		}
		while((n = getSLL_NODEnext(n))); // Step over nodes
	}

	fputc('}', fp);
	return;
}

// Outputs in the format: head->{5,6,2,9,1},tail->{1}
// Uses the display function of the list.
void displaySLLdebug(SLL *items, FILE *fp) {
	assert(items->display != NULL);

	fputs("head->", fp); // Start at the head
	displaySLL(items, fp);

	fputs(",tail->{", fp);
	if(items->size != 0) { // Skip if empty
		(items->display)(getSLL_NODEvalue(items->tail), fp); // Put the tail
	}
	fputc('}', fp);
	return;
}

// Frees the values using the freeing function of the list.
// Frees the nodes.
// Frees the SLL.
void freeSLL(SLL *items) {
	SLL_NODE *n; // Start at the head
	SLL_NODE *n2 = items->head; // Start at the head
	while((n = n2)) {
		if(items->free != NULL) {
			(items->free)(getSLL_NODEvalue(n)); //free the value
		}
		n2 = getSLL_NODEnext(n); // Save a pointer to the next node
		free(n); // Free the node
	}

	free(items); // Free the SLL
	return;
}



// Local functions to get nodes (not their values) from the list

// Returns a pointer to the node before the node at the given index.
// If the index is 0, returns a null pointer.
// Runs in constant time for nodes at a constant distance from the front.
static SLL_NODE *getPrevNodeSLL(SLL *items, int index) {
	assert(index >= 0);
	assert(index < items->size); // Would technically work with <=

	SLL_NODE *prev = NULL;

	if(index > 0) {
		SLL_NODE *n = items->head;
		int cur;
		for(cur = 0; cur < index; cur++) {
			// Step over the list until the node with the target index is reached.
			prev = n;
			n = getSLL_NODEnext(n);
		}
	}

	return prev;
}

// Returns a pointer to the node at the given index.
// Runs in constant time for nodes at the very back of the list.
// Runs in constant time for nodes at a constant distance from the front.
static SLL_NODE *getNodeSLL(SLL *items, int index) {
	assert(index >= 0);
	assert(index < items->size);

	// At the very back of the list?
	if(index == items->size - 1) {
		return items->tail;
	}

	if(index == 0) {
		return items->head;
	}

	// Note that the node is the node after the previous node.
	return getSLL_NODEnext(getPrevNodeSLL(items, index));
}


// Storage of nodes

struct sllnode {
	void *value;
	void *next;
};

static SLL_NODE *newSLL_NODE(void *value, SLL_NODE *next) {
	SLL_NODE *n = malloc(sizeof(SLL_NODE));
	assert(n != NULL); // The memory allocated shall not be null.
	n->value = value;
	n->next = next;
	return n;
}

static void *getSLL_NODEvalue(SLL_NODE *n) {
	// assert(n->value != NULL);
	return n->value;
}

static SLL_NODE *getSLL_NODEnext(SLL_NODE *n) {
	return n->next;
}

static void setSLL_NODEvalue(SLL_NODE *n, void *value) {
	n->value = value;
	return;
}

static void setSLL_NODEnext(SLL_NODE *n, SLL_NODE *next) {
	n->next = next;
	return;
}
