#include <stdlib.h>
#include <stdbool.h>

#include "pipe.c"

typedef struct Faculty {
    pid_t pid;
    Pipe *pipe;
    int score;
    int health;
    int tokens;
    bool eliminated;
} Faculty;

Faculty *new_faculty() {
    Faculty *self = malloc(sizeof(Faculty));

    self->pipe = new_pipe();
    self->score = 0;
    self->health = 10;
    self->tokens = 0;
    self->eliminated = false;

    return self;
}
