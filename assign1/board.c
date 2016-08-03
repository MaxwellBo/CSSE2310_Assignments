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

void print_board(Board *self) {
    for (int i = 0; i < self->height; i++) {
        for (int j = 0; j < self->width; j++) {
            printf("%c", self->board[i][j]->contents);
        }
        printf("\n");
    }
}

Node *get_node(Board *self, int x, int y) {
    return self->board[y][x];
}

