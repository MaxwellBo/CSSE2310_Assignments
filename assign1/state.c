#include <stdlib.h>
#include <stdio.h>

typedef struct State {
    int next_player;
    int row_for_O;
    int col_for_O;
    int move_number_for_O;
    int row_for_X;
    int col_for_X;
    int move_number_for_X;
} State;

State *new_State(void);
State *load_state(char *filename);
void free_state(State *self);

State *new_State(void) {

    State *self = malloc(sizeof(State));

    self->next_player = 0;
    self->row_for_O = 0;
    self->col_for_O = 0;
    self->move_number_for_O = -1;
    self->row_for_X = 0;
    self->col_for_X = 0;
    self->move_number_for_X = -1;

    return self;
}

State *load_state(char *filename) {
    State *self = malloc(sizeof(State));

    return self;
}

void free_state(State *self) {
    free(self);
}

int get_move_number_for(State *self, char pebble) {
    if (pebble == 'O') {
        return self->move_number_for_O;
    } else {
        return self->move_number_for_X;
    }
}

void incr_move_number_for(State *self, char pebble) {
    if (pebble == 'O') {
        (self->move_number_for_O)++;
    } else {
        (self->move_number_for_X)++;
    }
}

void set_row_for(State *self, char pebble, int x) {
    if (pebble == 'O') {
        self->row_for_O = x;
    } else {
        self->row_for_X = x;
    }
}

void set_col_for(State *self, char pebble, int x) {
    if (pebble == 'O') {
        self->col_for_O = x;
    } else {
        self->col_for_X = x;
    }
}

