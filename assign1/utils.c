#include <stdlib.h>
#include <stdio.h>

// TODO: Change this function name
char *read_line(FILE *file) {
    char *result = malloc(sizeof(char));
    int position = 0;
    int next = 0;
    
    while (1) {
        next = fgetc(file);

        if (next == EOF || next == '\n') {
            result[position] = '\0';
            return result;
        } 
        else {
            result[position] = (char)next;
            position++;
        }
        result = realloc(result, position + 1);
    }
}
