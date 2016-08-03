#include <stdlib.h>
#include <stdio.h>

#include "node.c"

typedef struct Board {
    int height;
    int width;
    Node ***board;
} Board;

Board *new_Board(int height, int width) {

    Board *self = malloc(sizeof(Board));

    self->height = height;
    self->width = width;
    self->board = malloc(sizeof(Node **) * height);

    for (int i = 0; i < height; ++i) {
        self->board[i] = malloc(sizeof(Node *) * width);
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            self->board[i][j] = new_Node('.');
        }
    }

    return self;
}

void free_board(Board *self) {
    for (int i = 0; i < self->height; i++) {
        for (int j = 0; j < self->width; j++) {
            free_node(self->board[i][j]);
        }
    }

    for (int i = 0; i < self->height; ++i) {
        free(self->board[i]);
    }

    free(self->board);

    free(self);
}

void print_board(Board *self) {
    for (int i = 0; i < self->height; i++) {
        for (int j = 0; j < self->width; j++) {
            printf("%c", self->board[i][j]->contents);
        }
        printf("\n");
    }
}

Node *get_node(Board *self, int x, int y) {
    if (!(0 <= x && x < self->width)) {
        return NULL;
    }

    if (!(0 <= y && y < self->height)) {
        return NULL;
    }

    return self->board[y][x];
}

void set_node(Board *self, int x, int y, char pebble) {
    // TODO: OOB
    // TODO: if occupied

    Node *staged = get_node(self, x, y);
    staged->contents = pebble;

    Node *above = get_node(self, x, y - 1);
    Node *below = get_node(self, x, y + 1);
    Node *to_left = get_node(self, x - 1, y);
    Node *to_right = get_node(self, x + 1, y);

    // Here we're just binding the nodes bidirectionally to any
    // neighbouring nodes
    if (above) { // AKA: Not a non-existent space
        staged->up = above;
        above->down = staged;
    } 

    if (below) {
        staged->down = below;
        below->up = staged;
    }

    if (to_left) {
        staged->left = to_left;
        to_left->right = staged;
    }

    if (to_right) {
        staged->right = to_right;
        to_right->left = staged;
    }

    Node *neighbours[] = { above, below, to_left, to_right };

    
    // Check if any enemy strings now have no liberties
    for (int i = 0; i < 4; i++) {
        if (neighbours[i] != NULL
            && neighbours[i]->contents != '.'
            && neighbours[i]->contents != pebble
            && !has_liberties(neighbours[i])) {
                printf("%s\n", "VICTORY CONDITION");
            }
    }
}
