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
    Faculty **faculties;
    Faculty *me;
    char label;
    int rerolls;
} State;

State *new_state(int numberOfPlayers, char label) {
    State *self = malloc(sizeof(State));

    self->faculties = malloc(sizeof(Faculty) * numberOfPlayers);

    for (int i = 0; i < numberOfPlayers; ++i) {
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

char *process_reroll(State *self, char *rolls) {
    // "they will reroll as many dice as possible as many times as possible"
    bool toReroll[6] = { true, true, true, true, true, true };

    int *tallys = tally_faces(rolls);

    // Then set the flags
    for (int i = 0; i < DICE; i++) {
        if (rolls[i] == '1' || rolls[i] == '2' || rolls[i] == '3') {
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

char *process_stay(State *self) {
    if (self->me->health < 5) {
        return make_string("go");
    } else {
        return make_string("stay");
    }

    char *response = make_string("stay");

    return response;
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
        char command[strlen("rerolled XXXXXX0")] = { 0 };
        sscanf(line, "%s ", command);

        char *response = NULL;
        // 0 on successful compare
        if (!strcmp(command, "turn")) {
            response = process_roll(state, &line[strlen("turn ")]);
        } else if (!strcmp(command, "rerolled")) {
            response = process_roll(state, &line[strlen("rerolled ")]);
        } else if (!strcmp(command, "rolled")) {
        } else if (!strcmp(command, "points")) {
        } else if (!strcmp(command, "attacks")) {
        } else if (!strcmp(command, "claim")) {
        } else if (!strcmp(command, "stay?")) {
            response = process_stay(state);
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
