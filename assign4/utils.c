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
 * Given an exit code, return the corresponding error message to be printed
 * to stderr. These are the error messages for 2310team.c and associated
 * files.
 *
 * - Pure
 *
 * A pointer to a statically allocated string containing the error message.
 */
char *get_error_message_2310team(int errno) {
    switch (errno) {
        default:
            return "";
    }
}

/*
 * Given an exit code, return the corresponding error message to be printed
 * to stderr. These are the error messages for 2310controller.c and associated
 * files.
 *
 * - Pure
 *
 * A pointer to a statically allocated string containing the error message.
 */
char *get_error_message_2310controller(int errno) {
    switch (errno) {
        default:
            return "";
    }
}
