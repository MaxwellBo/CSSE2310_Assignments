#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "files.c"

// Get globals
Team *team;
Sinister *sinister;

char *process_message(char *query) {

	Vec *narrative = new_vec();
	// TODO: Move this into a non-idiotic location

	// TODO: Make this number not magic
	char command[128];
    sscanf(query, "%s ", command);

    char *response = malloc(sizeof(char) * 128);

	if (!strcmp(command, "fightmeirl")) {
		char teamname[128];
		sscanf(query, "fightmeirl %s", teamname);

		char narrative_line[128];
		sprintf(narrative_line, "%s has a difference of opinion", teamname);

		append(narrative, clone_string(narrative_line));

		sprintf(response, "haveatyou %s\n", team->name);

		return response;
	} else if (!strcmp(command, "haveatyou")) {
		char teamname[128];
		sscanf(query, "haveatyou %s", teamname);

		char narrative_line[128];
		sprintf(narrative_line, "%s has a difference of opinion", teamname);

		append(narrative, clone_string(narrative_line));

		sprintf(response, "picking first agent %s\n", "pikachu");

		return response;
	}


	for (int i = 0; i < narrative->size; i++) {
		fprintf(stderr, "%s\n", narrative->data[i]);
	}

	// 19: TODO: Move this into a seperate util folder
	perror(get_error_message_2310team(19));
	exit(19);

	return response;

}
