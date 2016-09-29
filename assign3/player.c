#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "faculty.c"
#include "utils.c"

#define PLAYER 0
#define NUMBER_OF_PLAYERS 1
#define MY_ID 2
#define DICE 6

typedef struct State {
    int playerCount;
    Faculty **faculties;
    Faculty *me;
    char label;
    int rerolls;
} State;

State *new_state(int playerCount, char label) {
    State *self = malloc(sizeof(State));

    self->playerCount = playerCount;
    self->faculties = malloc(sizeof(Faculty) * playerCount);

    for (int i = 0; i < playerCount; ++i) {
        self->faculties[i] = new_faculty();
    }

    self->me = self->faculties[label - 'A'];
    self->label = label;
    self->rerolls = 0;

    return self;
}


char *build_response(bool *toReroll) {
    char *response = malloc(sizeof(char) * strlen("reroll XXXXXX0"));
    memset(response, '\0', sizeof(char) * strlen("reroll XXXXXX0"));
    strcpy(response, "reroll ");

    char *start = &response[strlen("reroll ")];

    for (int i = 0; i < DICE; i++) {
        if (toReroll[i]) {
            char dieNo = i + '1';
            *start = dieNo;
            start++;
        }
    }

    return response;
}

#ifdef EAIT
char *process_reroll(State *self, char *rolls) {

    if (strlen(rolls) != 6) {
        fprintf(stderr, "%s\n", get_error_message_player(5));
        exit(5);
    }

    // "they will reroll as many dice as possible as many times as possible"
    bool toReroll[6] = { true, true, true, true, true, true };

    int *tallys = tally_faces(rolls);

    // Then set the flags
    for (int i = 0; i < DICE; i++) {
        if (rolls[i] == '1' || rolls[i] == '2' || rolls[i] == '3') {
            // Check the tally
            int index = rolls[i] - '1';
            if (tallys[index] > 2) {
                toReroll[i] = false;
            }
        } else if (rolls[i] == 'H' && self->me->health < 6) {
            toReroll[i] = false;
        }
    }

    free(tallys);

    return build_response(toReroll);
}

char *process_stay(State *self) {
    if (self->me->health < 5) {
        return make_string("go");
    } else {
        return make_string("stay");
    }
}
#endif

#ifdef SCIENCE
char *process_reroll(State *self, char *rolls) {
    bool toReroll[6] = { true, true, true, true, true, true };

    for (int i = 0; i < DICE; i++) {

        // If they have less than 5 health, 
        // they will keep any H they get and reroll everything else
        if (self->me->health < 5) {
            if (rolls[i] == 'H') {
                toReroll[i] = false;
            }
        } else {
            // they will keep any A they get and reroll everthing else.
            if (rolls[i] == 'A') {
                toReroll[i] = false;
            }
        }
    }

    return build_response(toReroll);
}

char *process_stay(State *self) {
    // This player will retreat from StLucia immediately.
    return make_string("go");
}
#endif

char *process_roll(State *self, char *rolls) {
    if (self->rerolls < 2) {
        self->rerolls++;
        return process_reroll(self, rolls);
    } else {
        self->rerolls = 0;

        // Heal up
        int *tallys = tally_faces(rolls);
        give_Hs(self->me, tallys[3]);
        free(tallys);

        return make_string("keepall");
    }
}


void process_attack(State *self, char *line) {

    char attacker;
    int damage;
    char direction[strlen("out0")];
    direction[3] = '\0';

    sscanf(line, "%c %d %s\n", &attacker, &damage, &direction[0]);

    for (int i = 0; i < self->playerCount; i++) {
        if (!strcmp(direction, "in")) {
            if (self->faculties[i]->inStLucia) {
                give_As(self->faculties[i], damage);
            }
        } else {
            if (!self->faculties[i]->inStLucia) {
                give_As(self->faculties[i], damage);
            }
        }
    }
}

void process_claim(State *self, char *line) {
    // Remove any occupants from StLucia
    for (int i = 0; i < self->playerCount; i++) {
        self->faculties[i]->inStLucia = false;
    }

    char claimant;
    sscanf(line, "%c\n", &claimant);

    // Set the correct target of the attack
    self->faculties[claimant - 'A']->inStLucia = true;
}

void process_eliminated(State *self, char *line) {
    char eliminated;
    sscanf(line, "%c\n", &eliminated);

    // Defensively elimate all players that share that label
    for (int i = 0; i < self->playerCount; i++) {

        // Do they share the same index in the array?
        if (i == (eliminated - 'A')) {
            self->faculties[i]->eliminated = true;
        }
    }

    if (self->label == eliminated) {
        exit(0);
    }
}


// IMPURE
void validate_args(int argc, char **argv) {
    int status;

    if (argc != 3) {
        status = 1;
    } else if (!(2 <= atoi(argv[NUMBER_OF_PLAYERS]) 
        && atoi(argv[NUMBER_OF_PLAYERS]) <= 26)) {
        status = 2;
    } else if (strlen(argv[MY_ID]) != 1
        || !('A' <= argv[MY_ID][0] && argv[MY_ID][0] <= 'Z')) {
        status = 3;
    } else {
        return;
    }

    fprintf(stderr, "%s\n", get_error_message_player(status));
    exit(status);
}

void process_points(State *self, char *points) {
    if (atoi(points) < 1) {
        fprintf(stderr, "%s\n", get_error_message_player(5));
        exit(5);
    }
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {

    // Can terminate the program
    validate_args(argc, argv);

    fprintf(stdout, "%c", '!');

    State *state = new_state(atoi(argv[NUMBER_OF_PLAYERS]), argv[MY_ID][0]);

    while(1) {
        char *line = read_line(stdin);
        fprintf(stderr, "From StLucia:%s\n", line);

        // Max length
        char command[strlen("rerolled XXXXXX0")];
        memset(command, 0, strlen("rerolled XXXXXX0")); 
        sscanf(line, "%s ", command);

        char *response = NULL;
        // 0 on successful compare
        if (!strcmp(command, "turn")) {
            response = process_roll(state, &line[strlen("turn ")]);
        } else if (!strcmp(command, "rerolled")) {
            response = process_roll(state, &line[strlen("rerolled ")]);
        } else if (!strcmp(command, "rolled")) {
        } else if (!strcmp(command, "points")) {
            process_points(state, &line[strlen("points ")]);
        } else if (!strcmp(command, "attacks")) {
            process_attack(state, &line[strlen("attacks ")]);
        } else if (!strcmp(command, "eliminated")) {
            process_eliminated(state, &line[strlen("eliminated ")]);
        } else if (!strcmp(command, "claim")) {
            process_claim(state, &line[strlen("claim ")]);
        } else if (!strcmp(command, "stay?")) {
            response = process_stay(state);
        } else if (!strcmp(command, "winner") || !strcmp(command, "shutdown")) {
            exit(0);
        } else {
            fprintf(stderr, "%s\n", get_error_message_player(5));
            exit(5);
        }

        if (response != NULL) {
            fprintf(stdout, "%s\n", response);
            fflush(stdout);
            free(response);
        }
    }

    return 0;
}
