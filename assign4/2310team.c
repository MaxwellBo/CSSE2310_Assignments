#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utils.c"
#include "server.c"
#include "files.c"

#define CONTROLLERPORT 1
#define COMMAND 1
#define TEAMFILE 2
#define SINISTERFILE 3
#define TARGETPORT 4

#define MIN_ARGS 3
#define SIMULATION_ARGS 3

#define WAIT_ARGS 4

#define CHALLENGE_ARGS 5
#define MAX_ARGS 5


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
    } else if (argc == SIMULATION_ARGS && atoi(argv[CONTROLLERPORT]) < 1 ) {
        status = 6;
    } else if (argc == CHALLENGE_ARGS && atoi(argv[TARGETPORT]) < 1 ) {
        status = 6;
    }

    if (status) {
        fprintf(stderr, "%s\n", get_error_message_2310team(status)); 
        exit(status); 
    }

    return;
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {
    validate_args(argc, argv);

    if (argc == SIMULATION_ARGS) {
    } else if (argc == WAIT_ARGS) {
        Team *team = read_teamfile(argv[TEAMFILE]);
        read_sinisterfile(argv[SINISTERFILE]);
    } else if (argc == CHALLENGE_ARGS) {
        // Team *team = read_teamfile(argv[TEAMFILE]);
        // read_sinisterfile(argv[SINISTERFILE]);
    }

    // TODO: ephemeral port
    int port = 2000;

    fprintf(stdout, "%d\n", port);
    fflush(stdout);

    int fdServer = open_listen(port);
    process_connections(fdServer);

    return 0;
}
