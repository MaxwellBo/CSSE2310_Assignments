#include <stdlib.h>
#include <stdbool.h>

typedef struct Faculty {
    int score;
    int health;
    int tokens;
    bool eliminated;
    bool inStLucia;
} Faculty;

/**
 * Creates a model of a faculty.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated struct
 */
Faculty *new_faculty() {
    Faculty *self = malloc(sizeof(Faculty));

    self->score = 0;
    self->health = 10;
    self->tokens = 0;
    self->eliminated = false;
    self->inStLucia = false;

    return self;
}

/*
 * Imbue rolled "H" dice onto the faculty, leaving this function
 * to determine what to do with them depending on the state of the faculty
 */
void give_hs(Faculty *self, int number) {
    if (!self->inStLucia) {
        self->health += number;
    }

    if (self->health > 10) {
        self->health = 10;
    }
}

/*
 * Imbue rolled "A" dice onto the faculty, leaving this function
 * to determine what to do with them depending on the state of the faculty
 */
void give_as(Faculty *self, int number) {
    self->health -= number;

    if (self->health <= 0) {
        self->health = 0;
        self->eliminated = true;
    }
}
