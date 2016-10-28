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
    char *theirName;
    Sinister *sinister;
    Vec *narrative;
    Agent *mine;
    char *theirAgentName;
    AgentDetails *theirAgentDetails;
} Game;

Game *new_game() {
    Game *self = malloc(sizeof(Game));

    self->team = team;
    self->sinister = sinister;
    self->narrative = new_vec();
    self->mine = NULL;
    self->theirAgentDetails = NULL;

    return self;
}

/*---------------------------------------------------------------------------*/

void add_elimination_narrative_line(Game *self, char *eliminated) {
    char narrativeLine[128];
    sprintf(narrativeLine, "Team %s was eliminated.", eliminated);
    append(self->narrative, clone_string(narrativeLine));
}

void print_narrative(Game *self) {
    for (int i = 0; i < self->narrative->size; i++) {
        fprintf(stdout, "%s\n", (char *)self->narrative->data[i]);
    }

    fflush(stdout);
}

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

    add_elimination_narrative_line(self, self->team->name);
     
    // Abort when out of agents
    print_narrative(self);
    exit(0);
}

void attack(Game *self, char *response) {

    char *move = get_move(self->mine);
    sprintf(response, "attack %s %s\n", self->mine->name, move);

    // Get the type of the attack
    char *myAttackTypeName = get(self->sinister->attackToTypeName, move);

    // Get details regarding the type of the attack
    Type *type = (Type *)get(self->sinister->typeNameToType, myAttackTypeName);

    // Find out what its effectiveness is against their agent
    char *effectiveness = get(type->relations, self->theirAgentDetails->type);

    int damageValue = 0;

    if (effectiveness == NULL) {
        damageValue = 2;
    } else if (!strcmp(effectiveness, "+")) {
        damageValue = 3;
    } else {
        damageValue = 1;
    }

    // From sinisterfile: mammal it_worked_well ok something_went_wrong
    int index = 1 + (3 - damageValue);
    char *effectivenessString = type->effectiveness->data[index];

    // Log
    char narrativeLine[128];
    sprintf(narrativeLine, "%s uses attack: %s", self->mine->name, effectivenessString);
    append(self->narrative, clone_string(narrativeLine));

    cycle_move(self->mine);
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
        self->theirName = clone_string(teamName);

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
        self->theirName = clone_string(teamName);

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

        // Did something die?
        if (self->theirAgentDetails) {
            // Edit the last message so that it says that something died
            char narrativeLine[128];
            sprintf(narrativeLine, " - %s was eliminated.", self->theirAgentName);

            int lastIndex = self->narrative->size - 1;
            self->narrative->data[lastIndex] = concat(self->narrative->data[lastIndex], narrativeLine);
        }

        // Log
        char narrativeLine[128];
        sprintf(narrativeLine, "%s chooses %s", self->theirName, agentName);
        append(self->narrative, clone_string(narrativeLine));

        self->theirAgentName = agentName;
        self->theirAgentDetails = (AgentDetails *)get(self->sinister->agentNameToAgentDetails, agentName);

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
        char *theirAttackTypeName = get(self->sinister->attackToTypeName, attackName);

        // Get details regarding the type of the attack
        Type *type = (Type *)get(self->sinister->typeNameToType, theirAttackTypeName);

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


        char narrativeLine[128];

        if (self->mine->health <= 0) {
            choose_agent(self, response);

            sprintf(narrativeLine, "%s uses attack: %s - %s was eliminated", 
                agentName, effectivenessString, self->mine->name);

            append(self->narrative, clone_string(narrativeLine));

            return response;
        }

        // Log
        sprintf(narrativeLine, "%s uses attack: %s", agentName, effectivenessString);
        append(self->narrative, clone_string(narrativeLine));

        attack(self, response);
        return response;
    }

    // 19: TODO: Move this into a seperate util folder
    perror(get_error_message_2310team(19));
    exit(19);

    return response;
}



