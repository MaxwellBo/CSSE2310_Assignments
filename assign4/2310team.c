#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utils.c"

#include "server.c"
#include "client.c"

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

Team *team;
Sinister *sinister;

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
    } else if (argc == SIMULATION_ARGS && !(0 < atoi(argv[CONTROLLERPORT]) 
                && atoi(argv[CONTROLLERPORT]) <= 65535)) {
        status = 6;
    } else if (argc == CHALLENGE_ARGS && !(0 < atoi(argv[TARGETPORT]) 
                && atoi(argv[TARGETPORT]) <= 65535)) {
        status = 6;
    }

    if (status) {
        fprintf(stderr, "%s\n", get_error_message_2310team(status)); 
        exit(status); 
    }

    return;
}

void start_wait(int argc, char **argv) {
    team = read_teamfile(argv[TEAMFILE]);
    sinister = read_sinisterfile(argv[SINISTERFILE]);

    int port = 2000;

    fprintf(stdout, "%d\n", port);
    fflush(stdout);

    int fdServer = open_listen(port);
    process_connections(fdServer);

}

void start_challenge(int argc, char **argv) {
    team = read_teamfile(argv[TEAMFILE]);
    sinister = read_sinisterfile(argv[SINISTERFILE]);

    char *hostname = "127.0.0.1";

    struct in_addr* ipAddress = name_to_IP_addr(hostname);

    int fd = connect_to(ipAddress, atoi(argv[TARGETPORT]));
    process_connection(fd);
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
        start_wait(argc, argv);
    } else if (argc == CHALLENGE_ARGS) {
        start_challenge(argc, argv);
    }

    // TODO: ephemeral port

    return 0;
}

