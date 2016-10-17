#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct State {
} State;

/** 
 * TODO: DESCRIPTION
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated struct
 */
State *new_state() {
    State *self = malloc(sizeof(State));

    return self;
}

/**
 * Takes the argument count and argument array, and checks if they're valid.
 *
 * - Terminates the program if the program if invalid arguments are provided,
 *   and prints an appropriate error message to stderr
 */
void validate_args(int argc, char **argv) {
    int status;

    /* if (!((3 + MIN_PLAYERS) <= argc && argc <= (3 + MAX_PLAYERS))) { */
    /*     status = 1; */
    /* } else if (!(0 < atoi(argv[WINSCORE]))) { */
    /*     status = 2; */
    /* } else { */
    /*     return; */
    /* } */

    /* fprintf(stderr, "%s\n", get_error_message_2310team(status)); */
    /* exit(status); */
}

/**
 * Takes a FILE pointer and verifies whether it can be used as a sinisterfile.
 *
 * - Terminates the program if the sinisterfile either doesn't exist, or has
 *   invalid contents, and prints an appropriate error message to stderr
 */
void validate_sinisterfile(FILE *sinisterfile) {

    if (sinisterfile == NULL) {
    }
}


/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {
}

