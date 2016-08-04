#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

typedef struct Node {
    char contents;
    bool seen; 
    struct Node *up;
    struct Node *down;
    struct Node *left;
    struct Node *right;
} Node;

Node *new_Node(char contents);
void free_node(Node *self);
bool has_liberties(Node *self);

#endif // NODE_H
