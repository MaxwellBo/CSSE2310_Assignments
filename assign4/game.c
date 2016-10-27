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
    char *opponentName;
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

void choose_agent(Game *self, char *response) {
    // Respond

    for (int i = 0; i < 4; i++) { 
        Agent *agent = (Agent *)self->team->agents->data[i];

        if (agent->health > 0) {
            char narrative_line[128];
            sprintf(narrative_line, "%s chooses %s", self->team->name, agent->name);
            append(self->narrative, clone_string(narrative_line));

            sprintf(response, "iselectyou %s\n", agent->name);
            self->fighting = agent;

            return;
        }
    }

    // Abort when out of agents
    exit(0);
}

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
        self->opponentName = clone_string(teamname);

        // Log
        char narrative_line[128];
        sprintf(narrative_line, "%s has a difference of opinion", teamname);
        append(self->narrative, clone_string(narrative_line));

        // Response
        choose_agent(self, response);

        return response;
    } else if (!strcmp(command, "iselectyou")) {

        // // Parse
        // char agentname[128];
        // sscanf(query, "iselectyou %s", agentname);

        // // Log
        // char narrative_line[128];
        // sprintf(narrative_line, "", teamname);
        // append(self->narrative, clone_string(narrative_line));


        // // If we haven't chosen yet
        // if (!self->fighting) {
        //     choose_agent(self, response);
        //     return response;
        // } else {
            

        //     sprintf(response, "attack %s\n", team->name);
        // }
    }

    for (int i = 0; i < self->narrative->size; i++) {
        fprintf(stderr, "%s\n", self->narrative->data[i]);
    }

    // 19: TODO: Move this into a seperate util folder
    perror(get_error_message_2310team(19));
    exit(19);

    return response;
}


