#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

typedef struct Team {
    char *name;
    Vec *agents;
} Team;

Team *new_team() {
    Team *self = malloc(sizeof(Team));
    self->agents = new_vec();

    return self;
}

/*---------------------------------------------------------------------------*/

typedef struct Sinister {
    HashMap *typeNameToType;
    HashMap *attackToTypeName;
    Vec *agents;
} Sinister;

Sinister *new_sinister() {
    Sinister *self = malloc(sizeof(Sinister));

    return self;
}

/*---------------------------------------------------------------------------*/

typedef struct Type {
    Vec *effectiveness;
    HashMap *relations; 
} Type;

Type *new_type() {
    Type *self = malloc(sizeof(Type));

    return self;
}

/*---------------------------------------------------------------------------*/

typedef struct Agent {
    char *name;
    int health;
    Vec *moveSeq;
} Agent;

Agent *new_agent() {
    Agent *self = malloc(sizeof(Agent));
    self->health = 10;

    return self;
}

/*---------------------------------------------------------------------------*/

Team *read_teamfile(char *filename) {

    FILE *teamfile = fopen(filename, "r"); 
    Team *team = new_team();

    if (!teamfile) {
        fprintf(stderr, "%s\n", get_error_message_2310team(4));
        exit(4);
    }

    team->name = read_line(teamfile);

    // Get the next 4 lines
    for (int i = 0; i < 4; i++) {
        Vec *splits = split_read_line(teamfile);

        Agent *agent = new_agent();
        agent->name = clone_string(splits->data[0]);
        agent->moveSeq = splits;

        append(team->agents, agent);
    }

    return team;
}

Sinister *read_sinisterfile(char *filename) {
    FILE *sinisterfile = fopen(filename, "r"); 

    if (!sinisterfile) {
        fprintf(stderr, "%s\n", get_error_message_2310team(2));
        exit(2);
    }

    HashMap *typeNameToType = new_hashmap();
    HashMap *attackToTypeName = new_hashmap();
    
    /* ---------- TYPENAME ----------*/
    while (1) {
        char* line = read_line(sinisterfile);
        // fprintf(stderr, "%s\n", line);

        if (line[0] == '.') {
            break;
        }

        free(line);
    }

    /* ---------- EFFECTIVENESS ----------*/
    // Type *type = (Type *)get(typeNameToType, "australian");
    // Vec *effectiveness = type->effectiveness;
    while (1) {
        Vec *splits = split_read_line(sinisterfile);

        // for (int i = 0; i < splits->size; i++) {
        //     fprintf(stderr, "%s ", splits->data[i]);
        // }

        // fprintf(stderr, "%s\n", "");

        char *first = (char *)splits->data[0];

        if (first[0] == '#') {
            free_vec(splits);
        } else if (first[0] == '.') {
            free_vec(splits);
            break;
        } else {
            Type *type = new_type();
            type->effectiveness = splits;

            put(typeNameToType, clone_string(first), type);
            free_vec(splits);
        }
    }

    /* ---------- TYPE RELATIONS ---------- */
    // Type *type = (Type *)get(typeNameToType, "australian");
    // char *relation = get(type->relations, "bird");
    while (1) {
        Vec *splits = split_read_line(sinisterfile);

        // for (int i = 0; i < splits->size; i++) {
        //     fprintf(stderr, "%s ", splits->data[i]);
        // }

        // fprintf(stderr, "%s\n", "");

        char *first = (char *)splits->data[0];

        if (first[0] == '#') {
            free_vec(splits);
        } else if (first[0] == '.') {
            free_vec(splits);
            break;
        } else {
            Type *type = (Type *)get(typeNameToType, first);
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
    // char *type = get(attackToTypeName, "add_beetroot");
    while (1) {
        Vec *splits = split_read_line(sinisterfile);

        // for (int i = 0; i < splits->size; i++) {
        //     fprintf(stderr, "%s ", splits->data[i]);
        // }

        // fprintf(stderr, "%s\n", "");

        char *first = (char *)splits->data[0];

        if (first[0] == '#') {
            free_vec(splits);
        } else if (first[0] == '.') {
            free_vec(splits);
            break;
        } else {
            put(attackToTypeName, clone_string(first), clone_string(splits->data[1]));
            free_vec(splits);
        }
    }

    /* ---------- AGENTS ----------*/
    // char *type = get(attackToTypeName, "add_beetroot");
    while (1) {
        Vec *splits = split_read_line(sinisterfile);

        // for (int i = 0; i < splits->size; i++) {
        //     fprintf(stderr, "%s ", splits->data[i]);
        // }

        // fprintf(stderr, "%s\n", "");

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

    sinister->typeNameToType = typeNameToType;
    sinister->attackToTypeName = attackToTypeName;

    return sinister;
}
