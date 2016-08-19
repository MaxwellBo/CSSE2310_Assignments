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


/* Creates a state and fills it with default values and sentinel values
 * 
 * - Allocates memory
 *
 *   Returns the pointer to the newly alocated state 
 */
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

/*
 * Reads a state from a file with the matching filename
 *
 * - Performs IO to stederr and the filesystem
 * - Aborts the program if probems with the filesystem or file are
 *   encountered
 * - Allocates memory
 *
 * Return a pointer to a newly allocated state.
 */
State *read_state(char *filename) {
    State *staged = new_state(); 

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "%s\n", "Unable to open file");
        exit(4);
    }
    char *data = read_line(file); // mallocs

    // TODO: COUNT
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

/*
 * Wrties the state to a file with the matching filename, in append mode.
 *
 * - Performs IO the filesystem
 */
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

/*
 * Frees a state
 */
void free_state(State *self) {
    free(self);
}

/*
 * Getter for the move count of the player with the specified pebble
 *
 * Returns the movecount
 */
int get_move_number_for(State *self, char pebble) {
    if (pebble == 'O') {
        return self->moveNumberForO;
    } else {
        return self->move_number_for_X;
    }
}

/*
 * Increments the move count of the player with the specified pebble
 */
void incr_move_number_for(State *self, char pebble) {
    if (pebble == 'O') {
        (self->moveNumberForO)++;
    } else {
        (self->move_number_for_X)++;
    }
}

/*
 * Gets the row of the pebble to be played next of the player with the specified pebble
 *
 * Returns the row number (0 indexed)
 */
int get_row_for(State *self, char pebble) {
    if (pebble == 'O') {
        return self->rowForO;
    } else {
        return self->rowForX;
    }
}

/*
 * Gets the column of the pebble to be played next of the player with the specified pebble
 *
 * Returns the column number (0 indexed)
 */
int get_col_for(State *self, char pebble) {
    if (pebble == 'O') {
        return self->colForO;
    } else {
        return self->col_for_X;
    }
}


/*
 * Sets the row of the pebble to be played next of the player with the specified pebble
 */
void set_row_for(State *self, char pebble, int x) {
    if (pebble == 'O') {
        self->rowForO = x;
    } else {
        self->rowForX = x;
    }
}

/*
 * Sets the column of the pebble to be played next of the player with the specified pebble
 */
void set_col_for(State *self, char pebble, int x) {
    if (pebble == 'O') {
        self->colForO = x;
    } else {
        self->col_for_X = x;
    }
}

