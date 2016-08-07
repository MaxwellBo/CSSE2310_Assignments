#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.c"
#include "state.c"

#define MAX_LENGTH 70

#define HUMAN 0
#define COMPUTER 1

void generate_move(int *x, int *y, int move_count, 
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

    *x = r % G_h;
    *y = c % G_w;
}

int prompt_computer(Board *board, State *state, char pebble) {
    int failed_moves = 0;

    while (1) {
        int row;
        int col;

        generate_move(&row, 
                &col, 
                get_move_number_for(state, pebble) + failed_moves, 
                board->width, 
                board->height, 
                pebble);

        int status = set_node(board, col, row, pebble);

        if (status == STATUS_INVALID) {
            failed_moves++;
            continue;
        } else {
            printf("Player %c: %d %d\n", pebble, row, col);
            return status;
        }
    }
}

int prompt_human(Board *board, State *state, char pebble) {
    while (1) {

        char *filename = malloc(sizeof(char) * MAX_LENGTH);
        int row;
        int col;

        printf("Player %c> ", pebble);

        char *line = read_line(stdin);

        int assigned_filename = sscanf(line, "w%s", filename);
        int assigned_dimensions = sscanf(line, "%d %d", &row, &col);

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


void start_game(Board *board, State *state, int p1type, int p2type) {
   
    char first_player = state->next_player ? 'O' : 'X';
    char second_player = state->next_player ? 'X' : 'O';

    // Where HUMAN is 0, where COMPUTER is 1
    // Both of these functions have side effects on IO and state
    int (*prompts[2])(Board *, State *, char) = { &prompt_human, 
        &prompt_computer };
   
    int current_player;
    int board_status;

    while(1) {
        current_player = !(state->next_player);

        print_board(board);
        incr_move_number_for(state, first_player);

        board_status = (*prompts[p1type])(board, state, first_player);
        
        if (board_status == STATUS_VICTORY) {
            print_board(board);
            printf("Player %c wins\n", first_player);

            free_state(state);
            free_board(board);

            exit(0);
        }

        state->next_player = !state->next_player;
        current_player = !(state->next_player);
        
        print_board(board);
        incr_move_number_for(state, second_player);

        board_status = (*prompts[p2type])(board, state, second_player);

        if (board_status == STATUS_VICTORY) {
            print_board(board);
            printf("Player %c wins\n", second_player);

            free_state(state);
            free_board(board);

            exit(0);
        }

        state->next_player = !state->next_player;
    }
}


void debug(void) {
    printf("%s\n", "DEBUG ROUTINE");

    State *state = read_state("test.txt");
    Board *board = read_board("test.txt");

    print_board(board);

    write_dimensions(board, "write.txt");
    write_state(state, "write.txt");
    write_board(board, "write.txt");

    state = read_state("write.txt");
    board = read_board("write.txt");

    print_board(board);

    free_state(state);
    free_board(board);

}

int main(int argc, char **argv) {

    int p1type;
    int p2type;

    // TODO: REMOVE BEFORE HANDING IN
    if (argc == 1) {
        debug();
        return 0;
    }

    if (!(argc == 3 || argc == 4 || argc == 5)) {
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
        if (4 <= atoi(argv[3]) && atoi(argv[3]) <= 1000) {
            height = atoi(argv[3]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            exit(3);
        }

        if (4 <= atoi(argv[4]) && atoi(argv[4]) <= 1000) {
            width = atoi(argv[4]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            exit(3);
        }

        State *state = new_state();
        Board *board = new_board(height, width);  

        // can terminate the program
        start_game(board, state, p1type, p2type);

    } else {
        State *state = read_state(argv[3]);
        Board *board = read_board(argv[3]);

        // can terminate the program
        start_game(board, state, p1type, p2type);
    } 
}


