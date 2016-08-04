#include "node.h"

typedef struct Board {
    int height;
    int width;
    Node ***board; // TODO: Figure out why I can't declare an array of arrays
} Board;
