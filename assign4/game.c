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
    self->fighting = NULL;

    return self;
}

/*---------------------------------------------------------------------------*/

void choose_agent(Game *self, char *response) {
    for (int i = 0; i < 4; i++) { 
        Agent *agent = (Agent *)self->team->agents->data[i];

        if (agent->health > 0) {
            char narrativeLine[128];
            sprintf(narrativeLine, "%s chooses %s", self->team->name, agent->name);
            append(self->narrative, clone_string(narrativeLine));

            sprintf(response, "iselectyou %s\n", agent->name);
            self->fighting = agent;

            return;
        }
    }

    // Abort when out of agents
    exit(0);
}

void attack(Game *self, char *response) {
    sprintf(response, "attack %s %s\n", self->fighting->name, self->fighting->moveSeq->data[1]);

    // Log TODO RESULT STRING?
    char narrativeLine[128];
    sprintf(narrativeLine, "%s uses attack: SOMETHING", self->fighting->name);
    append(self->narrative, clone_string(narrativeLine));
}

char *process_message(Game *self, char *query) {

    // TODO: Make this number not magic
    char command[128];
    sscanf(query, "%s ", command);

    char *response = malloc(sizeof(char) * 128);

    if (!strcmp(command, "fightmeirl")) {
        // Parse
        char teamName[128];
        sscanf(query, "fightmeirl %s", teamName);
        self->opponentName = clone_string(teamName);

        // Log
        char narrativeLine[128];
        sprintf(narrativeLine, "%s has a difference of opinion", teamName);
        append(self->narrative, clone_string(narrativeLine));

        // Respond
        sprintf(response, "haveatyou %s\n", team->name);

        return response;
    } else if (!strcmp(command, "haveatyou")) {

        // Parse
        char teamName[128];
        sscanf(query, "haveatyou %s", teamName);
        self->opponentName = clone_string(teamName);

        // Log
        char narrativeLine[128];
        sprintf(narrativeLine, "%s has a difference of opinion", teamName);
        append(self->narrative, clone_string(narrativeLine));

        // Response
        choose_agent(self, response);

        return response;
    } else if (!strcmp(command, "iselectyou")) {
        // Parse
        char agentName[128];
        sscanf(query, "iselectyou %s", agentName);

        // Log
        char narrativeLine[128];
        sprintf(narrativeLine, "%s chooses agent %s", self->opponentName, agentName);
        append(self->narrative, clone_string(narrativeLine));

        // If we haven't chosen yet
        if (!self->fighting) {
            choose_agent(self, response);
            return response;
        } else {
            attack(self, response);
            return response;
        }
    } else if (!strcmp(command, "attack")) {
        // Parse
        char agentName[128];
        char attackName[128];
        sscanf(query, "attack %s %s", agentName, attackName);

        // Log
        char narrativeLine[128];
        sprintf(narrativeLine, "%s uses attack: SOMETHING", agentName);
        append(self->narrative, clone_string(narrativeLine));

        self->fighting->health -= 1;

        // Get the type of the attack
        char *opponentAttackTypeName = get(self->sinister->attackToTypeName, attackName);

        // Get details regarding the type of the attack
        Type *type = (Type *)get(self->sinister->typeNameToType, opponentAttackTypeName);

        // Find out what my agents type is
        
        // Find out what its effectiveness is against my agent
        char *effectiveness = get(type->relations, self->fighting->type);

        fprintf(stderr, "%s\n", effectiveness);

        if (self->fighting->health <= 0) {
            choose_agent(self, response);
            return response;
        }

        attack(self, response);
        return response;
    }


    for (int i = 0; i < self->narrative->size; i++) {
        fprintf(stdout, "%s\n", self->narrative->data[i]);
    }

    fflush(stdout);

    // 19: TODO: Move this into a seperate util folder
    perror(get_error_message_2310team(19));
    exit(19);

    return response;
}


