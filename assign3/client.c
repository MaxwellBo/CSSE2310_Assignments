#include <stdlib.h>

#include "pipe.c"
#include "faculty.c"

typedef struct Client {
    pid_t pid;
    Pipe *pipe;
    Faculty *faculty;
} Client;

Client *new_client() {
    Client *self = malloc(sizeof(Client));

    self->pipe = new_pipe();
    self->faculty = new_faculty();

    return self;
}
