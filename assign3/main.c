#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

char *get_error_message(int errno) {
	switch (errno) {
		case 1:
			return "Usage: stlucia rollfile winscore prog1 prog2 [prog3 [prog4]]";
		case 2:
			return "Invalid score";
		case 3:
			return "Unable to access rollfile";
		case 4:
			return "Error reading rolls";
		case 5:
			return "Unable to start subprocess";
		case 6:
			return "Player quit";
		case 7:
			return "Invalid message received from player";
		case 8:
			return "Invalid request by player";
		case 9:
			return "SIGINT caught";
		default:
			return "";
	}
}


/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {

	int toChildPipe[2];
    int toParentPipe[2];

    pipe(toChildPipe);
    pipe(toParentPipe);

	if (fork() == 0) {
		close(toChildPipe[WRITE_DESCRIPTOR]);
		close(toParentPipe[READ_DESCRIPTOR]);
		// dup2
		printf("%s\n", "Hello from child");
 		execl("/usr/bin/ls", "ls", "-al", 0);
 	} else {
 		close(toChildPipe[READ_DESCRIPTOR]);
 		close(toParentPipe[WRITE_DESCRIPTOR]);

	 	wait(NULL);
	 	printf("%s\n", "Hello from parent");
 	}
 	

    return 0;
}
