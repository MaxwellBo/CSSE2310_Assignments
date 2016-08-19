#include <stdlib.h>
#include <stdio.h>

#include "node.c"
#include "utils.c"

#define STATUS_SUCCESS 0
#define STATUS_INVALID 1
#define STATUS_VICTORY 2
#define STATUS_SUDOKU 3

typedef struct Board {
    int height;
    int width;
    Node ***board; 
} Board;

int set_node(Board *, int, int, char);

/*
 * Given a height and width, validates whether they lie between the limits of 
 * the board dimensions.
 */
bool are_valid_dimensions(int height, int width) {

    bool heightOK = (4 <= height && height <= 1000);
    bool widthOK = (4 <= width && width <= 1000);
    
    return heightOK && widthOK;
}

/* Creates a board with the specified dimensions.
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated board.
 */ 
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

/*
 * Reads a board from a file with the matching filename
 *
 * - Performs IO to stderr and the filesystem
 * - Aborts the program if problems with the filesytem or file are
 *   encountered
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated board.
 */
Board *read_board(char *filename) {

    int height;
    int width;

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "%s\n", "Unable to open file");
        exit(4);
    }

    char *data = read_line(file); // mallocs
    int assignedDimensions = sscanf(data, "%d %d", &height, &width);
    free(data);

    if (!(assignedDimensions == 2 && are_valid_dimensions(height, width))) {
        fprintf(stderr, "%s\n", "Incorrect file contents");
        exit(5);
    } 

    Board *staged = new_board(height, width);

    for (int y = 0; y < staged->height; y++) {

        char *row = read_line(file);

        for (int x = 0; x < staged->width; x++) {
            if (!(row[x] == '.' || row[x] == 'O' || row[x] == 'X')) {
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

/*
 * Writes the height and width to a file with the matching filename.
 * The file is created if it does not exist, and is cleared if it exists.
 *
 * - Performs IO to the filesystem
 */
void write_dimensions(Board *self, char *filename) {
    FILE *file = fopen(filename, "w");

    fprintf(file, "%d %d ", self->height, self->width);

    fclose(file);
}

/*
 * Writes the board (the grid) to a file with the matching filename, in
 * append mode.
 *
 * - Performs IO to the filesystem
 */
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

/*
 * Frees a board, and the nodes contained within its datastructure
 */
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

/*
 * Prints a pretty representation of the board to stdout
 *
 * - Performs IO to stdout
 */
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

/*
 * A getter for nodes contained within the board. The board is 0 indexed
 * from the top left hand corner. NOTE: This argument order is the opposite
 * of the (row, column) ordering.
 *
 * Returns a pointer to the requested node, or a NULL pointer if the requested
 * coordinate is OOB
 */
Node *get_node(Board *self, int x, int y) {
    if (!(0 <= x && x < self->width)) {
        return NULL;
    }

    if (!(0 <= y && y < self->height)) {
        return NULL;
    }

    return self->board[y][x];
}

/*
 * A setter for nodes contained within the board. The board is 0 indexed
 * from the top left hand corner. NOTE: This argument order is the opposite
 * of the (row, column) ordering.
 *
 * Takes a pebble and the coordinates where the pebble is to be placed.
 *
 * Returns an integer representing the status of the board after the 
 * pebble is placed, indicating OOB, or invalid placement, or a victory
 * condition
 * */
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
    Node *toLeft = get_node(self, x - 1, y);
    Node *toRight = get_node(self, x + 1, y);

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

    if (toLeft) {
        staged->left = toLeft;
        toLeft->right = staged;
    }

    if (toRight) {
        staged->right = toRight;
        toRight->left = staged;
    }

    Node *victoryNodes[] = { above, below, toLeft, toRight };

    // Check if any enemy strings now have no liberties
    for (int i = 0; i < 4; i++) {
        if (victoryNodes[i] != NULL
            && victoryNodes[i]->contents != '.'
            && victoryNodes[i]->contents != pebble
            && !has_liberties(victoryNodes[i])) {
                return STATUS_VICTORY;
        }
    }

    Node *lossNodes[] = { above, below, toLeft, toRight, staged };

    // Check if the player committed sudoku
    for (int i = 0; i < 5; i++) {
        if (lossNodes[i] != NULL
            && lossNodes[i]->contents != '.'
            && lossNodes[i]->contents != invert_pebble(pebble)
            && !has_liberties(lossNodes[i])) {
            return STATUS_SUDOKU;
        }
    }

    return STATUS_SUCCESS;
}
