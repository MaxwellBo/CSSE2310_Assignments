#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char *process_message(char *query) {

	// TODO: Make this number not magic
	char command[128];
    sscanf(query, "%s ", command);

    char *response = malloc(sizeof(char) * 128);

	if (!strcmp(command, "fightmeirl")) {
		char teamname [128];
		sscanf(query, "fightmeirl %s", teamname);
		fprintf(stderr, "Extracted teamname: %s\n", teamname);

		sprintf(response, "haveatyou %s\n", "TEAMNAME");
		return response;
	}

	// 19: TODO: Move this into a seperate util folder
	perror(get_error_message_2310team(19));
	exit(19);

	return response;

}
