#include <stdlib.h>

#include "faculty.c"

#define DICE 6
#define REROLL_LENGTH 14
#define REROLL_DICE_OFFSET 7 
#define PROCESS_LENGTH 5

typedef struct State {
    Faculty **faculties;
    char me;
} State;

State *new_state(int numberOfPlayers, char me) {
    State *self = malloc(sizeof(State));

    self->faculties = malloc(sizeof(Faculty) * numberOfPlayers);

    for (int i = 0; i < numberOfPlayers; ++i) {
        self->faculties[i] = new_faculty();
    }

    self->me = me;

    return self;
}

char *process_roll(State *self, char *rolls) {
    char *response = malloc(sizeof(char) * REROLL_LENGTH);
    memset(response, '\0', REROLL_LENGTH);
    strcpy(response, "reroll ");

    bool toKeep[6] = { false, false, false, false, false, false };

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
                toKeep[i] = true;
            }
            // TODO: Health
        } else if (rolls[i] == 'H') {
            toKeep[i] = true;
        }
    }

    // Then build the string
    for (int i = 0; i < DICE; i++) {
        if (!toKeep[i]) {
            char dieNo = i + '1';
            response[REROLL_DICE_OFFSET + i] = dieNo;
        }
    }


    return response;
}

char *process_stay(State *self) {
    char *response = malloc(sizeof(char) * PROCESS_LENGTH);

    strcpy(response, "stay");

    return response;
}

int label_to_index(char me) {
    return me - 'A';
}