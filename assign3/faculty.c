#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "bipe.c"

typedef struct Faculty {
	pid_t pid;
	Bipe *bipe;
	int health;
	int score;
	bool eliminated;
} Faculty;

Faculty *new_faculty() {
	Faculty *self = malloc(sizeof(Faculty));

	self->bipe = new_bipe();
	self->health = 30; // TODO: Figure out what this number should be
	self->score = 0;
	self->eliminated = false;

	return self;
}