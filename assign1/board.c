#include <stdlib.h>
#include <stdio.h>

#include "node.c"
#include "utils.c"

#define STATUS_SUCCESS 0
#define STATUS_INVALID 1
#define STATUS_VICTORY 2

typedef struct Board {
    int height;
    int width;
    Node ***board; 
} Board;

int set_node(Board *, int, int, char);

Board *new_board(int height, int width) {

    Board *self = malloc(sizeof(Board));

    self->height = height;
    self->width = width;

    self->board = malloc(sizeof(Node **) * height);

    for (int i = 0; i < height; i++) {
        self->board[i] = malloc(sizeof(Node *) * width);

        for (int j = 0; j < width; j++) {
            self->board[i][j] = new_node('.');
        }
    }

    return self;
}

Board *read_board(char *filename) {

    int height;
    int width;

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "%s\n", "Unable to open file");
        exit(4);
    }

    char *data = read_line(file); // mallocs
    sscanf(data, "%d %d", &height, &width);
    free(data);

    Board *staged = new_board(height, width);

    for (int y = 0; y < staged->height; y++) {

        char *row = read_line(file);

        for (int x = 0; x < staged->width; x++) {
            if (row[x] != '.' || row[x] != 'O' || row[x] != 'X') {
                fprintf(stderr, "%s\n", "Incorrect file contents");
                exit(5);
            }

            set_node(staged, x, y, row[x]);
        }

        free(row);
    }

    fclose(file);

    return staged;
}

void write_dimensions(Board *self, char *filename) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "%d %d ", self->height, self->width);

    fclose(file);
}

void write_board(Board *self, char *filename) {
    FILE *file = fopen(filename, "a");

    for (int i = 0; i < self->height; i++) {

        for (int j = 0; j < self->width; j++) {
            fprintf(file, "%c", self->board[i][j]->contents);
        }

        fprintf(file, "\n");
    }

    fclose(file);
}

void free_board(Board *self) {
    for (int i = 0; i < self->height; i++) {
        for (int j = 0; j < self->width; j++) {
            free_node(self->board[i][j]);
        }

        free(self->board[i]);
    }

    free(self->board);

    free(self);
}

void print_board(Board *self) {

    printf("/");
    for (int i = 0; i < self->width; i++) {
        printf("-");
    }
    printf("\\\n"); // esacped \ and a newline

    for (int i = 0; i < self->height; i++) {
        printf("|");

        for (int j = 0; j < self->width; j++) {
            printf("%c", self->board[i][j]->contents);
        }

        printf("|\n");
    }

    printf("\\");
    for (int i = 0; i < self->width; i++) {
        printf("-");
    }
    printf("/\n");
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

int set_node(Board *self, int x, int y, char pebble) {
    Node *staged = get_node(self, x, y);
    
    if (staged == NULL) {
        // Can't set node OOB
        return STATUS_INVALID;
    }

    if (staged->contents != '.') {
        // Requested location not empty
        return STATUS_INVALID;
    }

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
                return STATUS_VICTORY;
            }
    }

    return STATUS_SUCCESS;
}
