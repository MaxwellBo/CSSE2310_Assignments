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

void free_node(Node *self) {
    free(self);
}

bool has_liberties(Node *self) {
    Node *neighbours[] = { self->up, self->down, self->left, self->right };

    if (self->seen) {
        // If a node has been seen before, it didn't have any liberties
        // and thus, does not have liberties
        self->seen = false;
        return false;
    } else {
        self->seen = true;
    }

    for (int i = 0; i < 4; i++) {
        // Walls aren't liberties
        if (neighbours[i] == NULL) {
            continue;
        }

        // Empty spaces are liberties
        if (neighbours[i]->contents == '.') {
            self->seen = false;
            return true;
        }

        // This string has liberties if any pebbles in the string have
        // liberties
        if (neighbours[i]->contents == self->contents
            && has_liberties(neighbours[i])) {
            self->seen = false;
            return true;
        }
    }
    
    self->seen = false;
    return false;
}
