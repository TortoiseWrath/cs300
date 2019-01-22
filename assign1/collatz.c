#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
	assert(argc>1); // Ensure an argument is given.
	int n = atoi(argv[1]);
	assert(n>0); // Ensure a positive integer is passed.
	int pid = fork();
	if(pid) {
		// Parent process.
		wait(NULL);
		printf("\n");
		return 0;
	}
	// Child process.
	printf("%i", n);
	while(n!=1) {
		n=n%2?n*3+1:n/2;
		printf(",%i", n);
	}
	return 0;
}
