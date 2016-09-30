#include <stdlib.h>
#include <stdio.h>

#define DICE 6

/* 
 * Reads a single line from stdin until EOF or a newline is encountered
 *
 * - Performs IO to stdin
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated string
 */
char *read_line(FILE *file) {
    char *result = malloc(sizeof(char));
    int position = 0;
    int next = 0;
    
    while (1) {
        next = fgetc(file);

        if (next == EOF || next == '\n') {
            result[position] = '\0';
            return result;
        } else {
            result[position] = (char)next;
            position++;
        }
        result = realloc(result, position + 1);
    }
}

/*
 * Takes a string, and makes it a memory allocated string.
 *
 * - Allocates memory
 *
 * Reutrns a pointer to the newly allocated string
 */
char *make_string(char *string) {
    char *collector = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(collector, string);
    return collector;
}

/* 
 * Gets the pebble of the opposite player, given a pebble of a player
 *
 * - Pure
 * 
 * Returns the char representing the pebble 
 */
char invert_pebble(char pebble) {
    if (pebble == 'O') {
        return 'X';
    } else {
        return 'O';
    }
}

/*
 * Takes a string of rolls and returns an array of 6 integers.
 * Each of the integer cells corresponds to a tally of 1, 2, 3, H, A and P,
 * in the string of rolls.
 *
 * - Allocates memory
 *
 * Returns a pointer to the start of the array
 */
int *tally_faces(char *rolls) {

    int *tallys = calloc(DICE, sizeof(int));

    // First, tally up the numbers
    for (int i = 0; i < DICE; i++) {
        if (rolls[i] == '1' || rolls[i] == '2' || rolls[i] == '3') {
            // Treat '1' as a 0 for indexing
            int index = rolls[i] - '1';
            tallys[index]++;
        } else if (rolls[i] == 'H') {
            tallys[3]++;
        } else if (rolls[i] == 'A') {
            tallys[4]++;
        } else if (rolls[i] == 'P') {
            tallys[5]++;
        }
    }

    return tallys;
}

/*
 * Given an exit code, return the corresponding error message to be printed
 * to stderr. These are the error messages for player.c and associated
 * files.
 *
 * A pointer to a statically allocated string containing the error message.
 */
char *get_error_message_player(int errno) {
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

/*
 * Given an exit code, return the corresponding error message to be printed
 * to stderr. These are the error messages for stlucia.c and associated
 * files.
 *
 * A pointer to a statically allocated string containing the error message.
 */
char *get_error_message_stlucia(int errno) {
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
