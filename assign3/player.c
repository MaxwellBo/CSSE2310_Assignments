#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "utils.c"
#include "state.c"

#define PLAYER 0
#define NUMBER_OF_PLAYERS 1
#define MY_ID 2

// PURE
char *get_error_message(int errno) {
    switch (errno) {
        case 1:
            return "Usage: player number_of_players my_id";
        case 2:
            return "Invalid player count";
        case 3:
            return "Invalid player ID";
        case 4:
            return "Unexpectedly lost contact with StLucia";
        case 5:
            return "Bad message from StLucia";
        default:
            return "";
    }
}

// IMPURE
void validate_args(int argc, char **argv) {
    int status;

    if (argc != 3) {
        status = 1;
    } else if (!(2 <= atoi(argv[NUMBER_OF_PLAYERS]) 
        && atoi(argv[NUMBER_OF_PLAYERS]) <= 26)) {
        status = 2;
    } else if (strlen(argv[MY_ID]) != 1
        || !('A' <= argv[MY_ID][0] && argv[MY_ID][0] <= 'Z')) {
        status = 3;
    } else {
        return;
    }

    fprintf(stderr, "%s\n", get_error_message(status));
    exit(status);
}


/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {

    // Can terminate the program
    validate_args(argc, argv);

    State *state = new_state(atoi(argv[NUMBER_OF_PLAYERS]), argv[MY_ID][0]);

    while(1) {
        char *line = read_line(stdin);
        fprintf(stderr, "From hub:%s\n", line);

        char command[16];
        sscanf(line, "%s", command);

        char *response;
        // 0 on successful compare
        if (!strcmp(command, "turn")) {
            // Only pass in the dice
            response = process_roll(state, &line[5]);
        } else if (!strcmp(command, "rerolled")) {
            response = process_roll(state, &line[7]);
        } else if (!strcmp(command, "stay?")) {
            response = process_stay(state);
        } else {
            fprintf(stderr, "%s\n", get_error_message(5));
            exit(5);
        }

        fprintf(stdout, "%s\n", response);
        fflush(stdout);
        free(response);
    }

    return 0;
}
