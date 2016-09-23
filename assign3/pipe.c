#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

typedef struct Pipe {
    int *toParent;
    int *toChild;
    FILE *inbox;
    FILE *outbox;
} Pipe;


Pipe *new_pipe() {
    Pipe *self = malloc(sizeof(Pipe));

    self->toParent = malloc(sizeof(int) * 2);
    self->toChild = malloc(sizeof(int) * 2);

    pipe(self->toParent);
    pipe(self->toChild);

    self->inbox = NULL;
    self->outbox = NULL;

    return self;
}

void use_as_parent(Pipe *self) {
    // Close the ends we won't use
    close(self->toParent[WRITE_DESCRIPTOR]);
    close(self->toChild[READ_DESCRIPTOR]);

    self->inbox = fdopen(self->toParent[READ_DESCRIPTOR], "r");
    self->outbox = fdopen(self->toChild[WRITE_DESCRIPTOR], "w");
}

void use_as_child(Pipe *self) {
    close(self->toParent[READ_DESCRIPTOR]);
    close(self->toChild[WRITE_DESCRIPTOR]);

    dup2(self->toParent[WRITE_DESCRIPTOR], STDOUT_FILENO);
    dup2(self->toChild[READ_DESCRIPTOR], STDIN_FILENO);
}
