#include <stdlib.h>
#include <stdbool.h>

#include "pipe.c"
#include "faculty.c"

typedef struct Client {
    pid_t pid;
    char label;
    Pipe *pipe;
    Faculty *faculty;
    bool shutdown;
} Client;

Client *new_client(char label) {
    Client *self = malloc(sizeof(Client));

    self->label = label;
    self->pipe = new_pipe();
    self->faculty = new_faculty();
    self->shutdown = false;

    return self;
}
