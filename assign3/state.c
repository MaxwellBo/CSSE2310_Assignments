#include <stdlib.h>

#include "faculty.c"

#define DICE 6

typedef struct State {
    Faculty **faculties;
    char me;
    int rerolls;
} State;

State *new_state(int numberOfPlayers, char me) {
    State *self = malloc(sizeof(State));

    self->faculties = malloc(sizeof(Faculty) * numberOfPlayers);

    for (int i = 0; i < numberOfPlayers; ++i) {
        self->faculties[i] = new_faculty();
    }

    self->me = me;
    self->rerolls = 0;

    return self;
}

Faculty *get_me(State *self) {
    return self->faculties[self->me - 'A'];
}

char *build_response(bool *toReroll) {
    char *response = malloc(sizeof(char) * strlen("reroll XXXXXX0"));
    memset(response, '\0', strlen("reroll XXXXXX0"));
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

    // 1 in the 1th column, 2 in the 2th, etc.
    int tallys[3] = { 0, 0, 0 };

    // If they have more than 2 of the same number (1, 2 or 3), 
    // they will keep them

    // First, tally up the numbers
    for (int i = 0; i < DICE; i++) {
        if (rolls[i] == '1' || rolls[i] == '2' || rolls[i] == '3') {

            // Treat '1' as a 0 for indexing
            int index = rolls[i] - '1';
            tallys[index]++;
        }
    }

    // Then set the flags
    for (int i = 0; i < DICE; i++) {
        if (rolls[i] == '1' || rolls[i] == '2' || rolls[i] == '3') {
            int index = rolls[i] - '1';
            if (tallys[index] > 2) {
                toReroll[i] = false;
            }
            // TODO: Health
        } else if (rolls[i] == 'H' && get_me(self)->health < 6) {
            toReroll[i] = false;
        }
    }

    return build_response(toReroll);
}

char *process_roll(State *self, char *rolls) {
    if (self->rerolls < 2) {
        self->rerolls++;
        return process_reroll(self, rolls);
    } else {
        char *response = malloc(sizeof(char) * strlen("keepall0"));
        strcpy(response, "keepall");
        self->rerolls = 0;
        return response;
    }
}

char *process_broadcast(State *self, char *line) {
 
}

char *process_stay(State *self) {
    // What size does this array have to be?
    char *response = malloc(sizeof(char) * strlen("stay0"));

    strcpy(response, "stay");

    return response;
}
