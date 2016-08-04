#include <stdlib.h>
#include <stdio.h>

#include "state.h"

typedef struct State {
    int next_player;
    int row_for_O;
    int col_for_O;
    int move_number_for_O;
    int row_for_X;
    int col_for_X;
    int move_number_for_X;
} State;


State *new_State(void) {

    State *self = malloc(sizeof(State));

    self->next_player = 0;
    self->row_for_O = 0;
    self->col_for_O = 0;
    self->move_number_for_O = 0;
    self->row_for_X = 0;
    self->col_for_X = 0;
    self->move_number_for_X = 0;


    return self;
}

State *load_state(char *filename) {
    State *self = malloc(sizeof(State));

    return self;
}

void free_state(State *self) {
    free(self);
}
