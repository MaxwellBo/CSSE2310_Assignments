#include <stdlib.h>
#include <stdio.h>

typedef struct VecString {
    int size;
    char **data;
} VecString;

VecString *new_vec_string() {
    VecString *self = malloc(sizeof(VecString));
    self->size = 0;
    self->data = malloc(sizeof(char *));

    return self;
}

void append(VecString *self, char *x) {
    self->data = realloc(self->data, sizeof(char *) * (self->size + 1));
    self->data[self->size] = x;
    self->size++;
}

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



VecString *split_read_line(FILE *file) {
    VecString *result = new_vec_string();
    char *line = read_line(file);
    int length = strlen(line);
    int head = 0;
    int pointer = 0;

    // Map ' ' to '\0'
    while (line[pointer] != '\0') {
        if (line[pointer] == ' ') {
            line[pointer] = '\0';
        }

        pointer++;
    }

    pointer = 0;

    while (pointer <= length) {
        if (line[pointer] == '\0') {
            append(result, make_string(&line[head]));
            head = pointer + 1;
        }

        pointer++;
    }

    free(line);
    return result;
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
