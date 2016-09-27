#include <stdlib.h>
#include <stdbool.h>

typedef struct Faculty {
    int score;
    int health;
    int tokens;
    bool eliminated;
    bool inStLucia;
} Faculty;

Faculty *new_faculty() {
    Faculty *self = malloc(sizeof(Faculty));

    self->score = 0;
    self->health = 10;
    self->tokens = 0;
    self->eliminated = false;
    self->inStLucia = false;

    return self;
}

void give_Hs(Faculty *self, int number) {
    if (!self->inStLucia) {
        self->health += number;
    }
}