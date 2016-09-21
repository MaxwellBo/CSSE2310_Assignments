#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.c"

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

    int pid=fork();

    if (pid != 0) {
    	// Parent reads from child
		close(toParentPipe[WRITE_DESCRIPTOR]);
		close(toChildPipe[READ_DESCRIPTOR]);

 		FILE* fromChild = fdopen(toParentPipe[READ_DESCRIPTOR], "r");
 		FILE* toChild = fdopen(toParentPipe[WRITE_DESCRIPTOR], "w");

 		char *line = read_line(fromChild);
 		printf("%s\n", line);
 		
 		// fprintf(toChild, "%s\n", "yelling at child");
 		// fflush(toChild);

    } else {
    	// Child yells at parent
		close(toParentPipe[READ_DESCRIPTOR]);
		close(toChildPipe[WRITE_DESCRIPTOR]);

	    dup2(toParentPipe[WRITE_DESCRIPTOR], STDOUT_FILENO);
	    dup2(toChildPipe[READ_DESCRIPTOR], STDIN_FILENO);

	    execl("./player", "player", 0);
    }
    return 0;
}
