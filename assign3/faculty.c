#include <stdlib.h>
#include <stdbool.h>

typedef struct Faculty {
    int score;
    int health;
    int tokens;
    bool eliminated;
} Faculty;

Faculty *new_faculty() {
    Faculty *self = malloc(sizeof(Faculty));

    self->score = 0;
    self->health = 10;
    self->tokens = 0;
    self->eliminated = false;

    return self;
}
