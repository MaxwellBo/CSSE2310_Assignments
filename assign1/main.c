#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.c"
#include "state.c"


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

    incr_move_number_for(state, pebble);

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

int prompt_player(Board *board, State *state, char pebble) {
    
    incr_move_number_for(state, pebble);

    while (1) {
        printf("Player %c> ", pebble);
        int row;
        int col;

        int args_assigned = scanf("%d %d", &row, &col);
        // TODO: Make it reprompt on partial string

        if (args_assigned != 2) {
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


void start_game(State *state, Board *board, char p1type, char p2type) {
    
    // if 0 (O), start with X, else, start with O
    char to_play_first = state->next_player ? 'X' : 'O';
    char to_play_second = state->next_player ? 'O' : 'X';
   
    int board_status;

    while(1) {
        print_board(board);
        // All prompt_x have side effects, on both IO and board, and state
        if (p1type == 'c') {
            board_status = prompt_computer(board, state, to_play_first);
        } else {
            board_status = prompt_player(board, state, to_play_first);
        }
        
        if (board_status == STATUS_VICTORY) {
            print_board(board);
            printf("Player %c wins\n", to_play_first);
            exit(0);
        }

        print_board(board);

        if (p2type == 'c') {
            board_status = prompt_computer(board, state, to_play_second);
        } else {
            board_status = prompt_player(board, state, to_play_second);
        }

        if (board_status == STATUS_VICTORY) {
            print_board(board);
            printf("Player %c wins\n", to_play_second);
            exit(0);
        }
    }
}


void debug(void) {
    printf("%s\n", "DEBUG ROUTINE");

    int row;
    int col;

    generate_move(&row, &col, 0, 7, 7, 'O');
    printf("%d %d\n", row, col);
    generate_move(&row, &col, 1, 7, 7, 'O');
    printf("%d %d\n", row, col);
    generate_move(&row, &col, 2, 7, 7, 'O');
    printf("%d %d\n", row, col);
}

int main(int argc, char **argv) {

    char p1type;
    char p2type;
    int height = 4;
    int width = 4;

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
        p1type = 'c';
    } else if (strcmp(argv[1], "h") == 0) {
        p1type = 'h';
    } else {
        fprintf(stderr, "%s\n", "Invalid player type");
        exit(2);
    }

    if (strcmp(argv[2], "c") == 0) {
        p2type = 'c';
    } else if (strcmp(argv[2], "h") == 0) {
        p2type = 'h';
    } else {
        fprintf(stderr, "%s\n", "Invalid player type");
        exit(2);
    }

    // Board dimensions invalid checked first
    if (argc == 5) {
        // atoi returns 0 if conversion fails
        if (4 <= atoi(argv[3]) && atoi(argv[3]) <= 100) {
            height = atoi(argv[3]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            exit(3);
        }

        if (4 <= atoi(argv[4]) && atoi(argv[4]) <= 100) {
            width = atoi(argv[4]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            exit(3);
        }
    } else {
        printf("%s\n", "LOADING FILE");
        State *state = load_state(argv[3]);
        Board *board = load_board(argv[3]);

        // can terminate the program
        start_game(state, board, p1type, p2type);
    } 
    
    State *state = new_state();
    Board *board = new_board(height, width);  

    // can terminate the program
    start_game(state, board, p1type, p2type);
         
    exit(0);
}


