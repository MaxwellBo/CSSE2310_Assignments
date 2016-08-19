#include <stdlib.h>
#include <stdio.h>

typedef struct State {
    int nextPlayer;
    int rowForO;
    int colForO;
    int moveNumberForO;
    int rowForX;
    int col_for_X;
    int move_number_for_X;
} State;


State *new_state(void) {

    State *self = malloc(sizeof(State));

    self->nextPlayer = 0;
    self->rowForO = -1;
    self->colForO = -1;
    self->moveNumberForO = 0; // The first turn will incr this to 0
    self->rowForX = -1;
    self->col_for_X = -1;
    self->move_number_for_X = 0;

    return self;
}

State *read_state(char *filename) {
    State *staged = new_state(); 

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "%s\n", "Unable to open file");
        exit(4);
    }
    char *data = read_line(file); // mallocs

    sscanf(data, "%*d %*d %d %d %d %d %d %d %d", 
            &staged->nextPlayer,
            &staged->rowForO,
            &staged->colForO,
            &staged->moveNumberForO,
            &staged->rowForX,
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
            self->nextPlayer, 
            self->rowForO, 
            self->colForO,
            self->moveNumberForO, 
            self->rowForX, 
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
        return self->moveNumberForO;
    } else {
        return self->move_number_for_X;
    }
}

void incr_move_number_for(State *self, char pebble) {
    if (pebble == 'O') {
        (self->moveNumberForO)++;
    } else {
        (self->move_number_for_X)++;
    }
}

int get_row_for(State *self, char pebble) {
    if (pebble == 'O') {
        return self->rowForO;
    } else {
        return self->rowForX;
    }
}

int get_col_for(State *self, char pebble) {
    if (pebble == 'O') {
        return self->colForO;
    } else {
        return self->col_for_X;
    }
}


void set_row_for(State *self, char pebble, int x) {
    if (pebble == 'O') {
        self->rowForO = x;
    } else {
        self->rowForX = x;
    }
}

void set_col_for(State *self, char pebble, int x) {
    if (pebble == 'O') {
        self->colForO = x;
    } else {
        self->col_for_X = x;
    }
}

