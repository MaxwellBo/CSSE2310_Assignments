#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.c"

#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

typedef struct Pipe {
    int *toParent;
    int *toChild;
    FILE *inbox;
    FILE *outbox;
} Pipe;

/**
 * Creates a Pipe struct and puts two pipes inside of it.
 *
 * - Allocates memory
 * - Can terminate the program when unable to create pipes
 *
 * Returns a pointer to the Pipe struct
 */
Pipe *new_pipe() {
    Pipe *self = malloc(sizeof(Pipe));

    self->toParent = malloc(sizeof(int) * 2);
    self->toChild = malloc(sizeof(int) * 2);

    // 0 on success
    if (pipe(self->toParent) || pipe(self->toChild)) {
        fprintf(stderr, "%s\n", get_error_message_stlucia(5));
        exit(5);
    }

    self->inbox = NULL;
    self->outbox = NULL;

    return self;
}

/**
 * Takes a Pipe struct, and opens the necessary FILEs for a parent
 * to talk to a child.
 *
 * - Can terminate the program when unable to open file descriptors
 */
void use_as_parent(Pipe *self) {
    // Close the ends we won't use
    close(self->toParent[WRITE_DESCRIPTOR]);
    close(self->toChild[READ_DESCRIPTOR]);

    self->inbox = fdopen(self->toParent[READ_DESCRIPTOR], "r");
    self->outbox = fdopen(self->toChild[WRITE_DESCRIPTOR], "w");

    if (self->inbox == NULL || self->outbox == NULL) {
        fprintf(stderr, "%s\n", get_error_message_stlucia(5));
        exit(5);
    }
}

/**
 * Takes a Pipe struct, and mutates the file descriptor table so that stdin
 * and stdout are transferred into the pipe, and transfers stderr to
 * /dev/null.
 */
void use_as_child(Pipe *self) {
    close(self->toParent[READ_DESCRIPTOR]);
    close(self->toChild[WRITE_DESCRIPTOR]);

    int devNull = open("/dev/null", O_WRONLY);
    dup2(self->toParent[WRITE_DESCRIPTOR], STDOUT_FILENO);
    dup2(self->toChild[READ_DESCRIPTOR], STDIN_FILENO);
    dup2(devNull, STDERR_FILENO);
}
