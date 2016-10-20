#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utils.c"

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



typedef struct Team {
    char *name;
} Team;


typedef struct Type {
    Vec *effectiveness;
    HashMap *relations; 
} Type;


/** 
 * TODO: DESCRIPTION
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated struct
 */
Type *new_type() {
    Type *self = malloc(sizeof(Type));

    return self;
}

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


void read_teamfile(char *filename) {

    FILE *teamfile = fopen(filename, "r"); 

    if (!teamfile) {
        fprintf(stderr, "%s\n", get_error_message_2310team(4));
        exit(4);
    }

    // fprintf(stderr, "%s\n", read_line(teamfile));

    // Vec *splits = split_read_line(teamfile);

    // for (int i = 0; i < splits->size; i++) {
    //     fprintf(stderr, "%s\n", splits->data[i]);
    // }

}


void read_sinisterfile(char *filename) {
    FILE *sinisterfile = fopen(filename, "r"); 

    if (!sinisterfile) {
        fprintf(stderr, "%s\n", get_error_message_2310team(2));
        exit(2);
    }

    HashMap *dataByAnimal = new_hashmap();

    /* ---------- TYPENAME ----------*/
    while (1) {
        char* line = read_line(sinisterfile);
        fprintf(stderr, "%s\n", line);

        if (line[0] == '.') {
            break;
        }

        free(line);
    }

    /* ---------- EFFECTIVENESS ----------*/

    // Type *type = (Type *)get(dataByAnimal, "australian");
    // Vec *effectiveness = type->effectiveness;

    while (1) {
        Vec *splits = split_read_line(sinisterfile);

        for (int i = 0; i < splits->size; i++) {
            fprintf(stderr, "%s ", splits->data[i]);
        }

        fprintf(stderr, "%s\n", "");

        char *first = (char *)splits->data[0];

        if (first[0] == '#') {
            free_vec(splits);
        } else if (first[0] == '.') {
            free_vec(splits);
            break;
        } else {
            Type *type = new_type();
            type->effectiveness = splits;

            put(dataByAnimal, clone_string(first), type);
        }
    }

    /* ---------- TYPE RELATIONS ---------- */

    // Type *type = (Type *)get(dataByAnimal, "australian");
    // char *relation = get(type->relations, "bird");
    
    while (1) {
        Vec *splits = split_read_line(sinisterfile);

        for (int i = 0; i < splits->size; i++) {
            fprintf(stderr, "%s ", splits->data[i]);
        }

        fprintf(stderr, "%s\n", "");

        char *first = (char *)splits->data[0];

        if (first[0] == '#') {
            free_vec(splits);
        } else if (first[0] == '.') {
            free_vec(splits);
            break;
        } else {
            Type *type = (Type *)get(dataByAnimal, first);
            // NO NULL CHECK, HERE BE DRAGONS

            HashMap *relationByAnimal = new_hashmap();

            for (int i = 1; i < splits->size; i++) {

                char *relationThenAnimal = (char *)splits->data[i];

                char *relation = promote_char(relationThenAnimal[0]);
                char *animal = &relationThenAnimal[1];

                put(relationByAnimal, animal, relation);
            }

            type->relations = relationByAnimal;

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

    validate_args(argc, argv);

    if (argc == SIMULATION_ARGS) {
    } else if (argc == WAIT_ARGS) {
    } else if (argc == CHALLENGE_ARGS) {
        read_teamfile(argv[TEAMFILE]);
        read_sinisterfile(argv[SINISTERFILE]);
    }
}

