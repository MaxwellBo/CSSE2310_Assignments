#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "client.c"
#include "utils.c"

#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

#define NON_PLAYER_ARGS 3

#define MIN_PLAYERS 2
#define MAX_PLAYERS 26

#define ROLLFILE 1
#define WINSCORE 2

#define DICE 6

// PURE
char *get_error_message(int errno) {
    switch (errno) {
        case 1:
            return "Usage: stlucia rollfile winscore prog1 prog2 [prog3 [prog4]]";
        case 2:
            return "Invalid score";
        case 3:
            return "Unable to access rollfile";
        case 4:
            return "Error reading rolls";
        case 5:
            return "Unable to start subprocess";
        case 6:
            return "Player quit";
        case 7:
            return "Invalid message received from player";
        case 8:
            return "Invalid request by player";
        case 9:
            return "SIGINT caught";
        default:
            return "";
    }
}

// IMPURE
void validate_args(int argc, char **argv) {
    int status;

    if (!((3 + MIN_PLAYERS) <= argc && argc <= (3 + MAX_PLAYERS))) {
        status = 1;
    } else if (!(0 < atoi(argv[WINSCORE]))) {
        status = 2;
    } else {
        return;
    }

    fprintf(stderr, "%s\n", get_error_message(status));
    exit(status);
}

// IMPURE
void validate_rollfile(FILE *rollfile) {

    if (rollfile == NULL) {
        fprintf(stderr, "%s\n", get_error_message(3));
        exit(3);
    }

    while (1) {
        char c = fgetc(rollfile);

        if (c == EOF) { 
            rewind(rollfile);
            return;
        }

        if (!((c == '1') || (c == '2') || (c == '3') 
            || (c == 'H') || (c == 'A') || (c == 'P') || (c == '\n'))) {
            fprintf(stderr, "%s\n", get_error_message(4));
            exit(4);
      }
   }
}

// IMPURE
char get_roll(FILE *rollfile) {
    while(1) {
        char roll = fgetc(rollfile);
        
        if (roll == '\n') {
            continue;
        } else if (roll == EOF) {
            rewind(rollfile);
        } else {
            return roll;
        }
    }
}

int compare_rolls(const void *a, const void *b) {

    char charA = *((char *)a);
    char charB = *((char *)b);

    // Treat 'A' as 'J', so that 'H' < 'A' < 'P' when sorted (as 'H' < 'J')
    charA = (charA == 'A') ? 'J' : charA;
    charB = (charB == 'A') ? 'J' : charB;

    if (charA < charB) {
        return -1;
    } else if (charA == charB) {
        return 0;
    } else {
        return 1;
    }
}

// IMPURE
char *get_rolls(FILE *rollfile) {
    char *collector = malloc(sizeof(char) * DICE + 1);
    collector[DICE] = '\0';

    for (int i = 0; i < DICE; i++) {
        collector[i] = get_roll(rollfile);
    }

    // Sort the first 6 elems of the collector, leaving '\0' in place
    qsort(collector, 6, sizeof(char), compare_rolls);

    return collector;
}

char *get_rerolls(FILE *rollfile, char *rolls, char *rerolls) {

    // Copy so we don't mutate the rolls
    char *collector = malloc(sizeof(char) * DICE + 1);
    // dst, src, size
    strncpy(collector, rolls, DICE + 1);

    // Reroll the specified dice
    while (*rerolls != '\0') {
        int index = *rerolls - '1';
        collector[index] = get_roll(rollfile);
        rerolls++;
    }

    qsort(collector, 6, sizeof(char), compare_rolls);

    return collector;
}


void broadcast(int playerCount, Client **clients, char *message) {
    for (int i = 0; i < playerCount; i++) {
        fprintf(clients[i]->pipe->outbox, "%s\n", message);
        fflush(clients[i]->pipe->outbox);
    }
}

// IMPURE
void main_loop(FILE *rollfile, int winscore, int playerCount, Client **clients) {
    for (int i = 0; i < playerCount; i++) {

        char *rolls = get_rolls(rollfile);
        fprintf(clients[i]->pipe->outbox, "turn %s\n", rolls);
        fflush(clients[i]->pipe->outbox);

        while (1) {
            char *line = read_line(clients[i]->pipe->inbox);
            fprintf(stderr, "From child:%s\n", line);

            // Max length
            char command[strlen("eliminated0")];
            sscanf(line, "%s ", command);

            // 0 on successful compare
            if (!strcmp(command, "reroll")) {
                char *rerolls = get_rerolls(rollfile, rolls, &line[strlen("reroll ")]);
                fprintf(clients[i]->pipe->outbox, "rerolled %s\n", rerolls);
                fflush(clients[i]->pipe->outbox);
            } else if (!strcmp(command, "keepall")) {

                // char command = 

                // broadcast(playerCount, clients, )
                break;
            }
        }
    }
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {
    // ---------- VALIDATION ---------- 

    // Can terminate the program 
    validate_args(argc, argv);

    FILE *rollfile = fopen(argv[ROLLFILE], "r");

    // Can terminate the program
    validate_rollfile(rollfile);

    // ---------- MODEL CREATION ---------- 

    int playerCount = argc - NON_PLAYER_ARGS;

    Client **clients = malloc(sizeof(Client *) * playerCount);

    // ---------- FORKING ---------- 

    for (int i = 0; i < playerCount; i++) {
        clients[i] = new_client();

        pid_t pid = fork();
        clients[i]->pid = pid;

        if (pid == 0) {
            // ---------- CHILD ---------- 

            // Redirect stdin/stdout
            use_as_child(clients[i]->pipe);

            // A for (i = 0), B for (i = 1)
            char label[2] = { (char)i + 'A', '\0' };

            char playerCountArg[3];
            sprintf(playerCountArg, "%d", playerCount);
            playerCountArg[2] = '\0';

            execl(argv[NON_PLAYER_ARGS + i], "player", playerCountArg, label, NULL);
        } else {
            // ---------- PARENT ---------- 

            // Open file pointers to the child processes
            use_as_parent(clients[i]->pipe);
        }
    }

    // ---------- PARENT ---------- 
    main_loop(rollfile, atoi(argv[WINSCORE]), playerCount, clients);
}

