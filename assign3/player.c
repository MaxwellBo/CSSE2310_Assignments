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
    int eliminatedCount;
} State;

/**
 * With a player count and a label (which this player will be called),
 * creates a list of Facultys. 
 *
 * These Faculties are what the player *thinks* is the gamestate, to be
 * updated by St Lucia.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated struct
 */
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
    self->eliminatedCount = 0;

    return self;
}


/**
 * Given a State, gets the Faculty currently in StLucia.
 * 
 * - Pure
 *
 * Returns a pointer to the Faculty, or NULL, if there's no one in St Lucia.
 */
Faculty *get_stlucia(State *self) {
    for (int i = 0; i < self->playerCount; i++) {
        if (self->faculties[i]->inStLucia) {
            return self->faculties[i];
        }
    }

    return NULL;
}

/**
 * Given an array of 6 booleans ({true, false, false, false, false, true})
 * converts it into a null terminated string response ("reroll XXXXXX"),
 * with numbers corresponding to true booleans in the original array
 * ("reroll 16")
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
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

/**
 * With a State, takes a null-terminated string of rolls (eg "133HHA"),
 * and returns a boolean array of size 6, 
 * with a map from the rolls to whether they should be rerolled or not
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_reroll(State *self, char *rolls) {

    if (strlen(rolls) != 6) {
        fprintf(stderr, "%s\n", get_error_message_player(5));
        exit(5);
    }

    // "they will reroll as many dice as possible as many times as possible"
    bool toReroll[6] = {true, true, true, true, true, true};

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


/**
 * With a State, returns a response specifying whether the player
 * will "stay" or "go" in response to a "stay?" query.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_stay(State *self) {
    if (self->me->health < 5) {
        return make_string("go");
    } else {
        return make_string("stay");
    }
}
#endif

#ifdef SCIENCE

/**
 * With a State, takes a null-terminated string of rolls (eg "133HHA"),
 * and returns a boolean array of size 6, 
 * with a map from the rolls to whether they should be rerolled or not
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_reroll(State *self, char *rolls) {
    bool toReroll[6] = {true, true, true, true, true, true};

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

/**
 * With a State, returns a response specifying whether the player
 * will "stay" or "go" in response to a "stay?" query.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_stay(State *self) {
    // This player will retreat from StLucia immediately.
    return make_string("go");
}
#endif

#ifdef HABS

/**
 * With a State, takes a null-terminated string of rolls (eg "133HHA"),
 * and returns a boolean array of size 6, 
 * with a map from the rolls to whether they should be rerolled or not
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_reroll(State *self, char *rolls) {
    // If they have less than 5 health, then they will reroll any As they have.
    // Apart from that, they will not reroll.
    bool toReroll[6] = {false, false, false, false, false, false};

    for (int i = 0; i < DICE; i++) {

        // If they have less than 5 health, 
        // then they will reroll any As they have.
        if (self->me->health < 5) {
            if (rolls[i] == 'A') {
                toReroll[i] = true;
            }
        }
    }

    return build_response(toReroll);
}

/**
 * With a State, returns a response specifying whether the player
 * will "stay" or "go" in response to a "stay?" query.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_stay(State *self) {
    int aliveCount = self->playerCount - self->eliminatedCount;

    // This player will not retreat unless they have less than 4 health
    // If there is only one other player left, they will never retreat.
    if (self->me->health < 4 && !(aliveCount < 3)) {
        return make_string("go");
    } else {
        return make_string("stay");
    }
}
#endif

#ifdef HASS

/**
 * With a State, takes a null-terminated string of rolls (eg "133HHA"),
 * and returns a boolean array of size 6, 
 * with a map from the rolls to whether they should be rerolled or not
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_reroll(State *self, char *rolls) {
    // They will reroll everything else
    bool toReroll[6] = {true, true, true, true, true, true};


    int *tallys = tally_faces(rolls);

    for (int i = 0; i < DICE; i++) {
        // If they are in StLucia, they will never reroll Ps
        if (self->me->inStLucia) {
            if (rolls[i] == 'P') {
                toReroll[i] = false;
            }
        } else {
            if (get_stlucia(self) 
                    && rolls[i] == 'A' 
                    && tallys[4] >= get_stlucia(self)->health) {
                toReroll[i] = false;
            }
        }
    }


    free(tallys);
    return build_response(toReroll);
}

/**
 * With a State, returns a response specifying whether the player
 * will "stay" or "go" in response to a "stay?" query.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_stay(State *self) {
    // This player will not retreat from StLucia
    return make_string("stay");
}
#endif

#ifdef MABS

/**
 * With a State, takes a null-terminated string of rolls (eg "133HHA"),
 * and returns a boolean array of size 6, 
 * with a map from the rolls to whether they should be rerolled or not
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_reroll(State *self, char *rolls) {
    // They will reroll everything else
    bool toReroll[6] = {true, true, true, true, true, true};

    for (int i = 0; i < DICE; i++) {
        // they will always keep ... 3s
        if (rolls[i] == '3') {
            toReroll[i] = false;
        }

        if (rolls[i] == 'H' && !self->me->inStLucia) {
            toReroll[i] = false;
        }

        if (rolls[i] == 'A' && self->me->inStLucia) {
            toReroll[i] = false;
        }
    }

    return build_response(toReroll);
}

/**
 * With a State, returns a response specifying whether the player
 * will "stay" or "go" in response to a "stay?" query.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_stay(State *self) {
    // This player will retreat from StLucia immediately.
    return make_string("go");
}
#endif

/**
 * With a State, and given a string of null-terminated rolls ("133HHA")
 * determines the appropriate response to a "turn" or "rerolled" query
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string response
 */
char *process_roll(State *self, char *rolls) {

    char *response = process_reroll(self, rolls);

    if (self->rerolls < 2 && (strlen(response) != strlen("reroll "))) {
        self->rerolls++;

        return response;
    } else {
        self->rerolls = 0;

        // Heal up
        int *tallys = tally_faces(rolls);
        give_hs(self->me, tallys[3]);
        free(tallys);

        return make_string("keepall");
    }
}


/**
 * With a State, and given an attack query, updates the internal model
 * of other Faculties.
 *
 * - Mutates State
 */
void process_attack(State *self, char *line) {

    char attacker;
    int damage;
    char direction[strlen("out0")];
    direction[3] = '\0';

    sscanf(line, "%c %d %s\n", &attacker, &damage, &direction[0]);

    for (int i = 0; i < self->playerCount; i++) {
        if (!strcmp(direction, "in")) {
            if (self->faculties[i]->inStLucia) {
                give_as(self->faculties[i], damage);
            }
        } else {
            if (!self->faculties[i]->inStLucia) {
                give_as(self->faculties[i], damage);
            }
        }
    }
}

/**
 * With a State, and given an claim query, updates the internal model
 * of other Faculties, and moves the required Faculty to St Lucia.
 *
 * - Mutates State
 */
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

/**
 * With a State, and given an eliminated query, updates the internal model
 * of other Faculties, and moves the required Faculty to St Lucia.
 *
 * - Mutates State
 */
void process_eliminated(State *self, char *line) {
    char eliminated;
    sscanf(line, "%c\n", &eliminated);

    if ((eliminated) > ((self->playerCount - 1) + 'A')) {
        fprintf(stderr, "%s\n", get_error_message_player(5));
        exit(5);
    }

    // Defensively elimate all players that share that label
    for (int i = 0; i < self->playerCount; i++) {

        // Do they share the same index in the array?
        if (i == (eliminated - 'A')) {
            self->faculties[i]->eliminated = true;
        }
    }

    self->eliminatedCount++;

    if (self->label == eliminated) {
        exit(0);
    }
}


/**
 * Takes the argument count and argument array, and checks if they're valid.
 *
 * - Terminates the program if the program if invalid arguments are provided,
 *   and prints an appropriate error message to stderr
 */
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

/**
 * With a State, and given an eliminated points, updates the internal model
 * of other Faculties, and moves the required Faculty to St Lucia.
 *
 * - Mutates State
 */
void process_points(State *self, char *points) {
    /* if (atoi(points) < 1) { */
    /*     fprintf(stderr, "%s\n", get_error_message_player(5)); */
    /*     exit(5); */
    /* } */
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
        } else if (!strcmp(command, "winner") 
                || !strcmp(command, "shutdown")) {
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
