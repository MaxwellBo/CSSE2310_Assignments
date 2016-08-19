#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.c"
#include "state.c"

#define MAX_LENGTH 70

#define HUMAN 0
#define COMPUTER 1

/*
 * Given the number of the move that the move is being generated for
 * the dimensions of the board, and the color of the pebble,
 * mutate the row and column parameters for where the move is to be
 * placed
 */
void generate_move(int *row, int *col, int move_count, 
        int width, int height, char pebble) {

    int pebble_index;

    if (pebble == 'O') {
        pebble_index = 0;
    } else {
        pebble_index = 1;
    }

    // 0th for "O", 1th for "X"
    int I_r[] = { 1, 2 };
    int I_c[] = { 4, 10 };
    int F[] = { 29, 17 };
    int G_w = width;
    int G_h = height;
    int M = move_count;

    int B = I_r[pebble_index] * G_w + I_c[pebble_index];
    int r = I_r[pebble_index];
    int c = I_c[pebble_index];

    for (int i = 0; i <= M; i++) {
        if (i == 0) {
            // pass
        } else if (i % 5 == 0) {
            int N = (B + M/5 * F[pebble_index]) % 1000003;
            r = N / G_w;
            c = N % G_w;
        } else if (i % 5 == 1) {
            r += 1;
            c += 1;
        } else if (i % 5 == 2) {
            r += 2;
            c += 1;
        } else if (i % 5 == 3) {
            r += 1;
            c += 0;
        } else if (i % 5 == 4) {
            r += 0;
            c += 1;
        }
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
    int failed_moves = 0;

    while (1) {

        int status = set_node(board, col, row, pebble);

        if (status == STATUS_INVALID) {
            failed_moves++;

            generate_move(&row, 
                &col, 
                get_move_number_for(state, pebble) + failed_moves, 
                board->width, 
                board->height, 
                pebble);

            continue;
        } else {
            printf("Player %c: %d %d\n", pebble, row, col);

            incr_move_number_for(state, pebble);

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
 *
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

        int assigned_filename = sscanf(line, "w%70s", filename);
        int assigned_dimensions = sscanf(line, "%4d %4d", &row, &col);

        if (assigned_filename) {
            write_dimensions(board, filename);
            write_state(state, filename);
            write_board(board, filename);
        }
        
        free(filename);

        if (assigned_dimensions != 2) {
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
 *
 */
void start_game(Board *board, State *state, int p1type, int p2type) {
    // Where HUMAN is 0, where COMPUTER is 1
    // Both of these functions have side effects on IO and state
    int (*prompts[2])(Board *, State *, char) = { p1type ? &prompt_computer : &prompt_human,
    p2type ? &prompt_computer : &prompt_human };

    char players[] = { 'O', 'X' };

    int status;
    int index;

    while(1) {

        print_board(board);

        index = state->next_player;

        status = (*prompts[index])(board, state, players[index]);
        
        if (status == STATUS_VICTORY || status == STATUS_SUDOKU) {
            print_board(board);

            if (status == STATUS_VICTORY) {
                printf("Player %c wins\n", players[index]);
            }
            else {
                // Committed sudoku
                printf("Player %c wins\n", players[!index]);
            }

            free_state(state);
            free_board(board);

            exit(0);
        }

        state->next_player = !state->next_player;
    }
}


/* void debug(void) { */
/*     printf("%s\n", "DEBUG ROUTINE"); */
/*  */
/*     State *state = read_state("test.txt"); */
/*     Board *board = read_board("test.txt"); */
/*  */
/*     print_board(board); */
/*  */
/*     write_dimensions(board, "write.txt"); */
/*     write_state(state, "write.txt"); */
/*     write_board(board, "write.txt"); */
/*  */
/*     state = read_state("write.txt"); */
/*     board = read_board("write.txt"); */
/*  */
/*     print_board(board); */
/*  */
/*     free_state(state); */
/*     free_board(board); */
/*  */
/* } */

int main(int argc, char **argv) {

    int p1type;
    int p2type;

    /* // TODO: REMOVE BEFORE HANDING IN */
    /* if (argc == 1) { */
    /*     debug(); */
    /*     return 0; */
    /* } */

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
        if (areValidDimensions(atoi(argv[3]), atoi(argv[4]))) {
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

