#include <stdlib.h>

#include "faculty.c"
#include "utils.c"

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

    int *tallys = tally_faces(rolls);

    // Then set the flags
    for (int i = 0; i < DICE; i++) {
        if (rolls[i] == '1' || rolls[i] == '2' || rolls[i] == '3') {
            int index = rolls[i] - '1';
            if (tallys[index] > 2) {
                toReroll[i] = false;
            }
        } else if (rolls[i] == 'H' && get_me(self)->health < 6) {
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
        char *response = make_string("keepall");
        self->rerolls = 0;
        return response;
    }
}

void process_points(State *self, char *line) {
}

char *process_stay(State *self) {
    char *response = make_string("stay");

    return response;
}
