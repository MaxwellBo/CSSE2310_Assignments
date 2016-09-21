#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.c"

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {
	printf("%s\n", "Child starts");

	char *line = read_line(stdin);

	printf("Echoing: %s\n", line);

    return 0;
}
