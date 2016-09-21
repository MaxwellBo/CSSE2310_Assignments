#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.c"
#include "bipe.c"

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
	// argv[1] is rolls
	// argv[2] is points
	// Must be at least 2 players (argv[3] and argv[4])
	if (argc <= 4) {
		fprintf(stderr, "%s\n", get_error_message(1));
		exit(1);
	}

	int playerArgumentOffset = 3;
	int numberOfPlayers = argc - 3;

	pid_t pids[numberOfPlayers];
	Bipe *bipes[numberOfPlayers];

	for (int i = 0; i < numberOfPlayers; i++) {
		bipes[i] = new_bipe();

		if ((pids[i] = fork()) == 0) {
			use_as_child(bipes[i]);

			// Prevents loop in forked child
	    	execl("./player", "player", 0);
		}
	}

	// If the process has gotten to here, it's clearly a parent
	for (int i = 0; i < numberOfPlayers; i++) {
    	use_as_parent(bipes[i]);

 		fprintf(bipes[i]->outbox, "%s\n", "yelling at child");
 		fflush(bipes[i]->outbox);

 		char *line = read_line(bipes[i]->inbox);
 		printf("%s\n", line);

	}
}
