#include <stdlib.h>
#include <stdio.h>

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
