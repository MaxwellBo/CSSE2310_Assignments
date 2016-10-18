#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define CONTROLLERPORT 1
#define WAIT 1
#define TEAMFILE 2
#define SINISTERFILE 2
#define TARGETPORT 3

#define MIN_ARGS 3
#define MAX_ARGS 5

#define SIMULATION_ARGS 3
#define WAIT_ARGS 4
#define CHALLENGE_ARGS 5

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
    int status = 0;

    // if not within the valid range
    if (!((MIN_ARGS <= argc && argc <= MAX_ARGS))) {
        status = 1;
    }

    if (!fopen(argv[TEAMFILE], "r")) {
        status = 4;
    }

    if (argc == WAIT_ARGS || argc == CHALLENGE_ARGS) {
        if (!fopen(argv[SINISTERFILE], "r")) {
            status = 3;
        }
    }

    /* fprintf(stderr, "%s\n", get_error_message_2310team(status)); */
    /* exit(status); */
    return;
}


/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {


}

