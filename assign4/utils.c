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

char **split_read_line(FILE *file, int *allocated) {

    char **result = malloc(sizeof(char *));

    char *line = read_line(file);

    int position = 0;
    int startOfWord = 0;
    *allocated = 0;

    while (1) {
        if (line[position] == ' ' || line[position] == '\0') {
            result[allocated] = &line[startOfWord];
            (*allocated)++;

            if (line[position] == '\0') {
                return result;
            }

            result = realloc(result, *allocated * sizeof(char *));
            startOfWord = position + 1;
        }

        position++;
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
        case 1:
            return "Usage: 2310team controllerport teamfile\n\
                or: 2310team wait teamfile sinisterfile\n\
                or: 2310team challenge teamfile sinisterfile targetport";
        case 2:
            return "Unable to access sinister file";
        case 3:
            return "Error reading sinister file";
        case 4:
            return "Unable to access team file";
        case 5:
            return "Error reading team file";
        case 6:
            return "Invalid port number";
        case 7:
            return "Unable to connect to controller";
        case 8:
            return "Unable to connect to team";
        case 9:
            return "Unexpected loss of controller";
        case 10:
            return "Unexpected loss of team";
        case 19:
            return "Protocol error";
        case 20:
            return "System error";
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
