#include <stdlib.h>
#include <stdbool.h>

#include "pipe.c"

typedef struct Faculty {
	pid_t pid;
	Pipe *pipe;
	int health;
	int score;
	bool eliminated;
} Faculty;

Faculty *new_faculty() {
	Faculty *self = malloc(sizeof(Faculty));

	self->pipe = new_pipe();
	self->health = 30; // TODO: Figure out what this number should be
	self->score = 0;
	self->eliminated = false;

	return self;
}
