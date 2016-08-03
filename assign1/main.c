#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	if (!(argc == 3 || argc == 4 || argc == 5)) {
		fprintf(stderr, "%s\n", 
			"Usage: nogo p1type p2type [height width | filename]");
		return 1;
	}


	if (strcmp(argv[1], "c") == 0) {
		printf("%s\n", "Computer 1");
	} else if (strcmp(argv[1], "h") == 0) {
		printf("%s\n", "Human 1");
	} else {
		fprintf(stderr, "%s\n", "Invalid player type");
		return 2;
	}

	if (strcmp(argv[2], "c") == 0) {
		printf("%s\n", "Computer 2");
	} else if (strcmp(argv[2], "h") == 0) {
		printf("%s\n", "Human 2");
	} else {
		fprintf(stderr, "%s\n", "Invalid player type");
		return 2;
	}

	// atoi returns 0 if conversion fails
	if (4 <= atoi(argv[3]) && atoi(argv[3]) <= 100) {
		printf("%s\n", "Assigning height");
	}
	else {
		fprintf(stderr, "%s\n", "Invalid board dimension");
		return 3;
	}

	if (4 <= atoi(argv[3]) && atoi(argv[3]) <= 100) {
		printf("%s\n", "Assigning width");
	}
	else {
		fprintf(stderr, "%s\n", "Invalid board dimension");
		return 3;
	}

	printf("%s\n", "GAME STARTS PLAYING");

	return 0;
}
