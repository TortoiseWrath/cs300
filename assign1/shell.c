#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>

#define MAX_LINE 80
#define HISTORY_ENTRIES 10

struct history_entry {
	int index;
	char *args[MAX_LINE/2+1];
	struct history_entry *prev;
};

int main(void) {
	char *args[MAX_LINE/2+1];
	int should_run = 1;

	struct history_entry* history = NULL; // To store the history
	int history_full = 0;

	while(should_run) {
		printf("osh>");
		fflush(stdout);

		char line[MAX_LINE]; // To store the line of input
		fgets(line, MAX_LINE, stdin); // Read input.
		line[strcspn(line, "\n")] = 0; // Remove trailing newline from input.
	 	char *token = strtok(line, " "); // Tokenize input.
		int i = 0; // Keep track of which argument we're on.
		while(token) {
			args[i++] = token; // Store token in args array.
			token = strtok(NULL, " ");
		}
		args[i] = NULL;

		if(!args[0]) {
			continue;
		}
		else if(strcmp(args[0], "history") == 0) {
			// Display history
			struct history_entry *entry = history;
			if(entry == NULL) {
				printf("No commands in history.\n");
			}
			while(entry != NULL) {
				printf("%i ",entry->index);
				for(int i = 0; entry->args[i] != NULL; i++) {
					printf("%s ",entry->args[i]);
				}
				printf("\n");
				entry = entry->prev;
			}
			continue;
		}
		else if(args[0][0] == '!') {
			// Execute a command from history
			int n = 0;
			args[0]++;
			if(history == NULL) {
				printf("No commands in history.\n");
				continue;
			}
			else if(args[0][0] == '!') {
				n = history->index;
			}
			else if(!(n = atoi(args[0]))) {
				printf("Invalid integer: \"%s\"\n", args[0]);
				continue;
			}

			struct history_entry *entry = history;
			while(entry && entry->index != n) {
				entry = entry->prev;
			}
			if(!entry) {
				printf("No such command in history.\n");
				continue;
			}

			for(int i = 0; ; i++) { // Copy commands from history entry
				args[i] = entry->args[i];
				if(args[i] == NULL) break;
			}
		}

		int pid = fork();

		if(pid == 0) {
			// child process
			int i = 0;
			while(args[i] != NULL) i++; //find end of args
			if(strcmp(args[--i], "&") == 0) {
				args[i] = NULL; //do not pass the & to execvp
			}
			else if(args[i][strlen(args[i])-1] == '&') {
				args[i][strlen(args[i])-1] = 0;
			}
			execvp(args[0], args);
			return 0;
		}

		// parent process

		// Add command to history
		struct history_entry *command = malloc(sizeof(struct history_entry));
		for(int i = 0; ; i++) { // Copy arguments into history entry
			if(args[i] == NULL) {
				command->args[i] = NULL;
				break;
			}
			command->args[i] = malloc(strlen(args[i]));
			strcpy(command->args[i], args[i]);
		}
		if(history) {
			command->index = history->index + 1;
		}
		else {
			command->index = 1;
		}
		command->prev = history;
		history = command;
		// Check if the history is full and remove the oldest entries
		if(history_full) {
			struct history_entry *entry = history;
			// Navigate to the second entry.
			for(int i = 1; i < HISTORY_ENTRIES; i++) {
				entry = entry->prev;
			}
			// Destroy the first entry.
			for(int i = 0; entry->prev->args[i] != NULL; i++) {
				free(entry->prev->args[i]);
			}
			free(entry->prev);
			// Destroy the pointer to the first entry.
			entry->prev = NULL;
		}
		else if(history->index == HISTORY_ENTRIES) {
			history_full = 1;
		}

		// Check whether command ended with &
		int should_wait = 1;
		for(int i = 0; args[i] != NULL; i++) {
			should_wait = (args[i][strlen(args[i])-1] != '&');
		}
		if(should_wait) waitpid(pid, NULL, 0); // Wait for child process
	}

	return 0;
}
