#ifndef BOARD_H
#define BOARD_H

#include "node.h"

typedef struct Board {
    int height;
    int width;
    Node ***board; // TODO: Figure out why I can't declare an array of arrays
} Board;

Board *new_Board(int height, int width);
Board *load_Board(char *filename);
void free_board(Board *self);
void print_board(Board *self);
Node *get_node(Board *self, int x, int y);
void set_node(Board *self, int x, int y, char pebble);

#endif
