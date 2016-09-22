#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "utils.c"
#include "bipe.c"

#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

#define NON_PLAYER_ARGS 3

#define MIN_PLAYERS 2
#define MAX_PLAYERS 26

#define ROLLFILE 1
#define WINSCORE 2

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

void validate_args(int argc, char **argv) {

	bool invalidNumberOfArgs = !((3 + MIN_PLAYERS) <= argc && argc <= (3 + MAX_PLAYERS));

	// Returns 0 if atoi fails, which is invalid anyway
	bool invalidScore = !(0 < atoi(argv[WINSCORE]));

	int status;

	if (invalidNumberOfArgs) {
		status = 1;
	} else if (invalidScore) {
		status = 2;
	} else {
		return;
	}

	fprintf(stderr, "%s\n", get_error_message(status));
	exit(status);
}

FILE *get_rollfile(char *filepath) {
	
	FILE *rollfile = fopen(filepath, "r");

	if (rollfile == NULL) {
		fprintf(stderr, "%s\n", get_error_message(3));
		exit(3);
	}

	while (1) {
      	char c = fgetc(rollfile);

		if (c == EOF) { 
	   		return rollfile;
	   	}

     	if (!((c == '1') || (c == '2') || (c == '3') 
      		|| (c == 'H') || (c == 'A') || (c == 'P') || (c == '\n'))) {
      		fprintf(stderr, "%s\n", get_error_message(4));
      		exit(4);
      }
   }
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {

	// Can terminate the program 
	validate_args(argc, argv);

	// Can terminate the program
	FILE *rollfile = get_rollfile(argv[ROLLFILE]);

	int numberOfPlayers = argc - NON_PLAYER_ARGS;

	pid_t pids[numberOfPlayers];
	Bipe *bipes[numberOfPlayers];

	for (int i = 0; i < numberOfPlayers; i++) {
		bipes[i] = new_bipe();


		if ((pids[i] = fork()) == 0) {
			use_as_child(bipes[i]);

			// A for (i = 0), B for (i = 1)
			char label[2] = { (char)i + 'A', '\0' };

			char numberOfPlayersArg[3];
			sprintf(numberOfPlayersArg, "%d", numberOfPlayers);
			numberOfPlayersArg[2] = '\0';

			// Prevents loop in forked child
	    	execl(argv[NON_PLAYER_ARGS + i], "player", numberOfPlayersArg, label, NULL);
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

