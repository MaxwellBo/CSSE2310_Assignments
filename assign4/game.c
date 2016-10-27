#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "files.c"

// Redeclare globals
Team *team;
Sinister *sinister;

/*---------------------------------------------------------------------------*/

typedef struct Game {
    Team *team;
    Sinister *sinister;
    Vec *narrative;
    Agent *fighting;
} Game;

Game *new_game() {
    Game *self = malloc(sizeof(Game));

    self->team = team;
    self->sinister = sinister;
    self->narrative = new_vec();
    self->fighting;

    return self;
}

/*---------------------------------------------------------------------------*/

char *process_message(Game *self, char *query) {

    // TODO: Make this number not magic
    char command[128];
    sscanf(query, "%s ", command);

    char *response = malloc(sizeof(char) * 128);

    if (!strcmp(command, "fightmeirl")) {
        // Parse
        char teamname[128];
        sscanf(query, "fightmeirl %s", teamname);

        // Log
        char narrative_line[128];
        sprintf(narrative_line, "%s has a difference of opinion", teamname);
        append(self->narrative, clone_string(narrative_line));

        // Respond
        sprintf(response, "haveatyou %s\n", team->name);

        return response;
    } else if (!strcmp(command, "haveatyou")) {

        // Parse
        char teamname[128];
        sscanf(query, "haveatyou %s", teamname);

        // Log
        char narrative_line[128];
        sprintf(narrative_line, "%s has a difference of opinion", teamname);
        append(self->narrative, clone_string(narrative_line));

        // Respond
        Agent *firstUp = (Agent *)self->team->agents->data[0];
        sprintf(response, "iselectyou %s\n", firstUp->name);
        self->fighting = firstUp;

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
