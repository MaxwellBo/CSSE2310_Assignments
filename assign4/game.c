#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "files.c"

// Redeclare globals
Team *team;
Sinister *sinister;

/* -------------------- */

typedef struct Game {
    Team *team;
    Sinister *sinister;
    Vec *narrative;
} Game;

Game *new_game() {
    Game *self = malloc(sizeof(Game));

    self->team = team;
    self->sinister = sinister;
    self->narrative = new_vec();

    return self;
}
/* -------------------- */

char *process_message(Game *self, char *query) {

    // TODO: Make this number not magic
    char command[128];
    sscanf(query, "%s ", command);

    char *response = malloc(sizeof(char) * 128);

    if (!strcmp(command, "fightmeirl")) {
        char teamname[128];
        sscanf(query, "fightmeirl %s", teamname);

        char narrative_line[128];
        sprintf(narrative_line, "%s has a difference of opinion", teamname);

        append(self->narrative, clone_string(narrative_line));

        sprintf(response, "haveatyou %s\n", team->name);

        return response;
    } else if (!strcmp(command, "haveatyou")) {
        char teamname[128];
        sscanf(query, "haveatyou %s", teamname);

        char narrative_line[128];
        sprintf(narrative_line, "%s has a difference of opinion", teamname);

        append(self->narrative, clone_string(narrative_line));

        sprintf(response, "picking first agent %s\n", "pikachu");

        return response;
    }


    for (int i = 0; i < self->narrative->size; i++) {
        fprintf(stderr, "%s\n", self->narrative->data[i]);
    }

    // 19: TODO: Move this into a seperate util folder
    perror(get_error_message_2310team(19));
    exit(19);

    return response;

}
