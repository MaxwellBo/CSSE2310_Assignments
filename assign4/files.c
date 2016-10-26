#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Team {
    char *name;
} Team;

Team *new_team() {
    Team *self = malloc(sizeof(Team));

    return self;
}

typedef struct Sinister {
    HashMap *typeToDetails;
    HashMap *attackToType;
    Vec *agents;
} Sinister;

Sinister *new_sinister() {
    Sinister *self = malloc(sizeof(Sinister));

    return self;
}

typedef struct Type {
    Vec *effectiveness;
    HashMap *relations; 
} Type;

Type *new_type() {
    Type *self = malloc(sizeof(Type));

    return self;
}

typedef struct Agent {
    char *name;
    char *type;
    char *first;
    char *second;
    char *third;
} Agent;

Agent *new_agent() {
    Agent *self = malloc(sizeof(Agent));

    return self;
}

Team *read_teamfile(char *filename) {

    FILE *teamfile = fopen(filename, "r"); 
    Team *team = new_team();

    if (!teamfile) {
        fprintf(stderr, "%s\n", get_error_message_2310team(4));
        exit(4);
    }

    team->name = read_line(teamfile);

    return team;
}

Sinister *read_sinisterfile(char *filename) {
    FILE *sinisterfile = fopen(filename, "r"); 

    if (!sinisterfile) {
        fprintf(stderr, "%s\n", get_error_message_2310team(2));
        exit(2);
    }

    HashMap *typeToDetails = new_hashmap();
    HashMap *attackToType = new_hashmap();
    Vec *agents = new_vec();

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
    // Type *type = (Type *)get(typeToDetails, "australian");
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

            put(typeToDetails, clone_string(first), type);
            free_vec(splits);
        }
    }

    /* ---------- TYPE RELATIONS ---------- */
    // Type *type = (Type *)get(typeToDetails, "australian");
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
            Type *type = (Type *)get(typeToDetails, first);
            // NO NULL CHECK, HERE BE DRAGONS

            HashMap *relationByAnimal = new_hashmap();

            for (int i = 1; i < splits->size; i++) {

                char *relationThenAnimal = (char *)splits->data[i];

                char *relation = promote_char(relationThenAnimal[0]);
                char *animal = &relationThenAnimal[1];

                put(relationByAnimal, clone_string(animal), relation);
            }

            type->relations = relationByAnimal;

            free_vec(splits);
        }
    }

    /* ---------- ATTACKS ----------*/
    // char *type = get(attackToType, "add_beetroot");
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
            put(attackToType, clone_string(first), clone_string(splits->data[1]));
            free_vec(splits);
        }
    }

    /* ---------- AGENTS ----------*/
    // char *type = get(attackToType, "add_beetroot");
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
            // TODO: Either hash the splits, or just dump
            // them in a vec
        }
    }

    /* ---------- CLEANUP ----------*/
    Sinister *sinister = new_sinister();

    sinister->typeToDetails = typeToDetails;
    sinister->attackToType;

    return sinister;
}
