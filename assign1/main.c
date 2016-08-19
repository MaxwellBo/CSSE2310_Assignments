#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.c"
#include "state.c"

#define MAX_LENGTH 70

#define HUMAN 0
#define COMPUTER 1

/*
 * Given the previous row and column of the last moved placed, the number of
 * the move that the move is being generated for the dimensions of the board, 
 * and the color of the pebble, the function will mutate the row and 
 * column parameters for where the move is to be placed.
 * 
 */
void generate_move(int *row, int *col, int moveCount, 
        int width, int height, char pebble) {

    int pebbleIndex;

    if (pebble == 'O') {
        pebbleIndex = 0;
    } else {
        pebbleIndex = 1;
    }

    // 0th for "O", 1th for "X"
    int I_r[] = { 1, 2 };
    int I_c[] = { 4, 10 };
    int F[] = { 29, 17 };
    int G_w = width;
    int G_h = height;
    int M = moveCount;

    int B = I_r[pebbleIndex] * G_w + I_c[pebbleIndex];

    int r;
    int c;

    if (M == 0) {
        r = I_r[pebbleIndex];
        c = I_c[pebbleIndex];
    } else if (M % 5 == 0) {
        int N = (B + M/5 * F[pebbleIndex]) % 1000003;
        r = N / G_w;
        c = N % G_w;
    } else if (M % 5 == 1) {
        r = *row + 1;
        c = *col + 1;
    } else if (M % 5 == 2) {
        r = *row + 2;
        c = *col + 1;
    } else if (M % 5 == 3) {
        r = *row + 1;
        c = *col + 0;
    } else if (M % 5 == 4) {
        r = *row + 0;
        c = *col + 1;
    }

    *row = r % G_h;
    *col = c % G_w;
}

/*
 * Given a board and a game state and a pebble, ask the computer to make 
 * a move. The computer will fetch a move from the save file, attempt to
 * use it, and if failing that, generate more moves until a successful move
 * made. The computer will store generated moves and increment the movecount.
 *
 * - Mutates board and game state.
 *
 * Returns that status of the board, indicating game completion
 */
int prompt_computer(Board *board, State *state, char pebble) {
    
    int row = get_row_for(state, pebble);
    int col = get_col_for(state, pebble);

    while (1) {

        int status = set_node(board, col, row, pebble);
        incr_move_number_for(state, pebble);

        if (status == STATUS_INVALID) {

            generate_move(&row, 
                &col, 
                get_move_number_for(state, pebble), 
                board->width, 
                board->height, 
                pebble);

            continue;
        } else {
            printf("Player %c: %d %d\n", pebble, row, col);

            generate_move(&row, 
                &col, 
                get_move_number_for(state, pebble), 
                board->width, 
                board->height, 
                pebble);
            
            set_row_for(state, pebble, row);
            set_col_for(state, pebble, col);

            return status;
        }
    }
}

/*
 * Given a board and a game state and a pebble, ask the user to make 
 * move. The chosen move will be attempted, and if it fails the
 * player will be reprompted.
 *
 * - Mutates board and game state.
 * - Performs IO on stdout, stderr and the filesystem
 *
 * Returns that status of the board, indicating game completion.
 */
int prompt_human(Board *board, State *state, char pebble) {
    while (1) {

        printf("Player %c> ", pebble);

        char *line = read_line(stdin);

        if (line[0] == '\0') {
            fprintf(stderr, "%s\n", "End of input from user");
            free_board(board);
            free_state(state);
            exit(6);
        }

        char *filename = malloc(sizeof(char) * MAX_LENGTH + 1);
        int row;
        int col;

        int assignedFilename = sscanf(line, "w%70s", filename);
        int assignedDimensions = sscanf(line, "%4d %4d", &row, &col);

        if (assignedFilename) {
            write_dimensions(board, filename);
            write_state(state, filename);
            write_board(board, filename);
        }
        
        free(filename);

        if (assignedDimensions != 2) {
            continue; // Reprompt if invalid number of args
        }

        int status = set_node(board, col, row, pebble);
       
        if (status == STATUS_INVALID) {
            continue; // Reprompt if invalid movement
        } else {
            return status;
        }
    }
}

/*
 * Give a board, a gamestate and the types of each player, initializes 
 * the main game loop. This function multiplexes out the required prompt
 * functions to the respective players.
 *
 * - Performs IO on stdout
 * - Can terminate the program
 */
void start_game(Board *board, State *state, int p1type, int p2type) {
    // Where HUMAN is 0, where COMPUTER is 1
    // Both of these functions have side effects on IO and state
    int (*prompts[2])(Board *, State *, char) = { 
        p1type ? 
            &prompt_computer : &prompt_human,
        p2type ? 
            &prompt_computer : &prompt_human };

    char players[] = { 'O', 'X' };

    int status;
    int index;

    while(1) {

        print_board(board);

        index = state->nextPlayer;

        status = (*prompts[index])(board, state, players[index]);
        
        if (status == STATUS_VICTORY || status == STATUS_SUDOKU) {
            print_board(board);

            if (status == STATUS_VICTORY) {
                printf("Player %c wins\n", players[index]);
            } else {
                // Committed sudoku
                printf("Player %c wins\n", players[!index]);
            }

            free_state(state);
            free_board(board);

            exit(0);
        }

        state->nextPlayer = !state->nextPlayer;
    }
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {

    int p1type;
    int p2type;

    if (!(argc == 4 || argc == 5)) {
        fprintf(stderr, "%s\n", 
            "Usage: nogo p1type p2type [height width | filename]");
        exit(1);
    }

    if (strcmp(argv[1], "c") == 0) {
        p1type = COMPUTER;
    } else if (strcmp(argv[1], "h") == 0) {
        p1type = HUMAN;
    } else {
        fprintf(stderr, "%s\n", "Invalid player type");
        exit(2);
    }

    if (strcmp(argv[2], "c") == 0) {
        p2type = COMPUTER;
    } else if (strcmp(argv[2], "h") == 0) {
        p2type = HUMAN;
    } else {
        fprintf(stderr, "%s\n", "Invalid player type");
        exit(2);
    }

    // Board dimensions invalid checked first
    if (argc == 5) {
        int height;
        int width;

        // atoi returns 0 if conversion fails
        // TODO: Add isDigit check for the whole string
        if (are_valid_dimensions(atoi(argv[3]), atoi(argv[4]))) {
            height = atoi(argv[3]);
            width = atoi(argv[4]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            exit(3);
        }

        State *state = new_state();
        Board *board = new_board(height, width);

        int row;
        int col;

        generate_move(&row,
                &col, 
                get_move_number_for(state, 'O'), 
                board->width, 
                board->height, 
                'O');
        
        set_row_for(state, 'O', row);
        set_col_for(state, 'O', col);

        generate_move(&row,
                &col, 
                get_move_number_for(state, 'X'), 
                board->width, 
                board->height, 
                'X');

        set_row_for(state, 'X', row);
        set_col_for(state, 'X', col);

        // Can perform IO
        // Can terminate program
        start_game(board, state, p1type, p2type);


    } else {
        // Can perform IO
        // Can terminate program on invalid reads
        State *state = read_state(argv[3]);
        Board *board = read_board(argv[3]);

        // can terminate the program
        start_game(board, state, p1type, p2type);
    } 
}

