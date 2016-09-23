#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "utils.c"

#define PLAYER 0
#define NUMBER_OF_PLAYERS 1
#define MY_ID 2

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

void validate_args(int argc, char **argv) {

    bool invalidNumberOfArgs = argc != 3;

    bool invalidPlayerCount = !(2 <= atoi(argv[NUMBER_OF_PLAYERS]) 
        && atoi(argv[NUMBER_OF_PLAYERS]) <= 26);

    bool invalidPlayerID = strlen(argv[MY_ID]) != 1 || 
        !('A' <= argv[MY_ID][0] && argv[MY_ID][0] <= 'Z');

    int status;

    if (invalidNumberOfArgs) {
        status = 1;
    } else if (invalidPlayerCount) {
        status = 2;
    } else if (invalidPlayerID) {
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


    // printf("%s\n", "Child starts");
    char *line = read_line(stdin);

    fprintf(stdout, "%s %s %s\n", argv[PLAYER], argv[NUMBER_OF_PLAYERS], argv[MY_ID]);
    fflush(stdout);

    fprintf(stderr, "From hub:%s\n", line);

    return 0;
}

