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


State *new_state(void) {

    State *self = malloc(sizeof(State));

    self->next_player = 1;
    self->row_for_O = 0;
    self->col_for_O = 0;
    self->move_number_for_O = -1; // The first turn will incr this to 0
    self->row_for_X = 0;
    self->col_for_X = 0;
    self->move_number_for_X = -1;

    return self;
}

State *read_state(char *filename) {
    State *staged = new_state(); 

    FILE *file = fopen(filename, "r");
    char *data = read_line(file); // mallocs

    sscanf(data, "%*d %*d %d %d %d %d %d %d %d", 
            &staged->next_player,
            &staged->row_for_O,
            &staged->col_for_O,
            &staged->move_number_for_O,
            &staged->row_for_X,
            &staged->col_for_X,
            &staged->move_number_for_X
          );

    free(data);
    fclose(file);

    return staged;
}

void write_state(State *self, char *filename) {
    FILE *file = fopen(filename, "a");

    fprintf(file, "%d %d %d %d %d %d %d\n", 
            self->next_player, 
            self->row_for_O, 
            self->col_for_O,
            self->move_number_for_O, 
            self->row_for_X, 
            self->col_for_X, 
            self->move_number_for_X
           );

    fclose(file);
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

