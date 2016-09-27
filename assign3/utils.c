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

char *make_string(char *string) {
    char *collector = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(collector, string);
    return collector;
}

/* 
 * Gets the pebble of the opposite player, given a pebble of a player
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

int *tally_faces(char *rolls) {

    int *tallys = malloc(sizeof(int) * DICE);
    memset(tallys, 0, DICE);

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
