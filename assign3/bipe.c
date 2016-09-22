#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Biderection pipe? Geddit? Ha.
 */
#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

typedef struct Bipe {
    int *toParent;
    int *toChild;
    FILE *inbox;
    FILE *outbox;
} Bipe;


Bipe *new_bipe() {
	Bipe *self = malloc(sizeof(Bipe));

	self->toParent = malloc(sizeof(int) * 2);
	self->toChild = malloc(sizeof(int) * 2);

	pipe(self->toParent);
	pipe(self->toChild);

	self->inbox = NULL;
	self->outbox = NULL;

	return self;
}

void use_as_parent(Bipe *self) {
	// Close the ends we won't use
	close(self->toParent[WRITE_DESCRIPTOR]);
	close(self->toChild[READ_DESCRIPTOR]);

 	self->inbox = fdopen(self->toParent[READ_DESCRIPTOR], "r");
 	self->outbox = fdopen(self->toChild[WRITE_DESCRIPTOR], "w");
}

void use_as_child(Bipe *self) {
	close(self->toParent[READ_DESCRIPTOR]);
	close(self->toChild[WRITE_DESCRIPTOR]);

    dup2(self->toParent[WRITE_DESCRIPTOR], STDOUT_FILENO);
    dup2(self->toChild[READ_DESCRIPTOR], STDIN_FILENO);
}
