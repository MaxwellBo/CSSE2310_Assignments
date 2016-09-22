#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

void validates_args(int argc, char **argv) {
	int status;

	if (argc != 3) {
		status = 1;
	} else if (2 <= atoi(argv[1]) && atoi(argv[1]) <= 26) {
		status = 2;
	} else if (strlen(argv[2]) == 1 &&
		'A' <= argv[2][0] && argv[2][0] <= 'Z') {
		status = 3;
	} else {
		return;
	}

	fprintf(stderr, "%s\n", get_error_message(status));
	exit(status);
}

void validate_args_no(int argc, char **argv) {
}

void validate_players(int argc, char **argv) {
}

void validate_ID(int argc, char **argv) {

}