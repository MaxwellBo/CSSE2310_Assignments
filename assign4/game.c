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
    Agent *mine;
    Agent *theirs;
} Game;

Game *new_game() {
    Game *self = malloc(sizeof(Game));

    self->team = team;
    self->sinister = sinister;
    self->narrative = new_vec();
    self->mine = NULL;
    self->theirs = NULL;

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
            self->mine = agent;

            return;
        }
    }

    // Abort when out of agents
    exit(0);
}

void attack(Game *self, char *response) {
    sprintf(response, "attack %s %s\n", self->mine->name, get_move(self->mine));

    // Log TODO RESULT STRING?
    char narrativeLine[128];
    sprintf(narrativeLine, "%s uses attack: SOMETHING", self->mine->name);
    append(self->narrative, clone_string(narrativeLine));

    cycle_move(self->mine);
}

void print_narrative(Game *self) {
    for (int i = 0; i < self->narrative->size; i++) {
        fprintf(stdout, "%s\n", self->narrative->data[i]);
    }

    fflush(stdout);
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
        if (!self->mine) {
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

        // Get the type of the attack
        char *opponentAttackTypeName = get(self->sinister->attackToTypeName, attackName);

        // Get details regarding the type of the attack
        Type *type = (Type *)get(self->sinister->typeNameToType, opponentAttackTypeName);

        // Find out what my agents type is
        AgentDetails *agentDetails = (AgentDetails *)get(
            self->sinister->agentNameToAgentDetails, self->mine->name); 

        // Find out what its effectiveness is against my agent
        char *effectiveness = get(type->relations, agentDetails->type);

        int damageValue = 0;

        if (effectiveness == NULL) {
            damageValue = 2;
        } else if (!strcmp(effectiveness, "+")) {
            damageValue = 3;
        } else {
            damageValue = 1;
        }

        damage(self->mine, damageValue);

        // From sinisterfile: mammal it_worked_well ok something_went_wrong
        int index = 1 + (3 - damageValue);
        char *effectivenessString = type->effectiveness->data[index];

        // Log
        char narrativeLine[128];
        sprintf(narrativeLine, "%s uses attack: %s", agentName, effectivenessString);
        append(self->narrative, clone_string(narrativeLine));

        if (self->mine->health <= 0) {
            choose_agent(self, response);
            return response;
        }

        attack(self, response);
        return response;
    }

    // 19: TODO: Move this into a seperate util folder
    perror(get_error_message_2310team(19));
    exit(19);

    return response;
}



