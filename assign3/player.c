#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.c"

char *get_error_message(int errno) {
	switch (errno) {
		case 1:
			return "Usage: player number_of_players my_id";
		case 2:
			return "Invalid player count";
		case 3:
			return "Invalid player ID";
		case 4:
			return "Unexpectedly lost contact with StLucia";
		case 5:
			return "Bad message from StLucia";
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
	// printf("%s\n", "Child starts");
	char *line = read_line(stdin);

	fprintf(stdout, "%s %s %s\n", argv[1], argv[2], line);
	fflush(stdout);

    return 0;
}
