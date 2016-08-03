#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    char contents;
    bool seen; 
    struct Node *up;
    struct Node *down;
    struct Node *left;
    struct Node *right;
} Node;

Node *new_Node(char contents) {
    Node *self = malloc(sizeof(Node));
    
    self->contents = contents;
    self->seen = false;
    self->up = NULL;
    self->down = NULL;
    self->left = NULL;
    self->right = NULL;

    return self;
}

void free_Node(Node *self) {
    free(self);
}
