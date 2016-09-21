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
	// if (argc <= 4) {
	// 	fprintf(stderr, "%s\n", get_error_message(1));
	// 	exit(1);
	// }

	Bipe *bipe = new_bipe();

    int pid=fork();

    if (pid != 0) {
    	// Parent reads from child
    	use_as_parent(bipe);

 		fprintf(bipe->outbox, "%s\n", "yelling at child");
 		fflush(bipe->outbox);

 		char *line = read_line(bipe->inbox);
 		printf("%s\n", line);

    } else {
    	use_as_child(bipe);

	    execl("./player", "player", 0);
    }
    return 0;
}
