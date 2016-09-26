#include <stdlib.h>
#include <stdbool.h>

#include "faculty.c"

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

char *process_roll(char *rolls) {
    char *response = malloc(sizeof(char) * 16);

    strcpy(response, "reroll 123456");

    return response;
}

char *process_stay() {
    char *response = malloc(sizeof(char) * 5);

    strcpy(response, "stay");

    return response;
}
