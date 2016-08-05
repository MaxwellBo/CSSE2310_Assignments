#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.c"
#include "state.c"

void debug(void);
void start_game(State *state, Board *board, char p1type, char p2type);
void generate_move(int move_count, int width, int height, int pebble);
void prompt_computer(Board *board, State *state, char pebble);
void prompt_player(Board *board, State *state, char pebble);

void debug(void) {
    printf("%s\n", "DEBUG ROUTINE");

    Board *b1 = new_Board(5, 5);
    set_node(b1, 1, 1, 'X');
    set_node(b1, 0, 0, 'O');
    set_node(b1, 1, 0, 'X');
    set_node(b1, 0, 1, 'O');

    print_board(b1);

    set_node(b1, 0, 2, 'X');

    print_board(b1); // side effects

    free_board(b1);
}

void generate_move(int move_count, int width, int height, int pebble) {
    // 0th for "0", 1th for "X"
    int I_r[] = { 1, 2 };
    int I_c[] = { 4, 10 };
    int F[] = { 29, 17 };
    int G_w = width;
    int G_h = height;
    int M = move_count;

    int B = I_r[pebble] * G_w + I_c[pebble];
    int r = I_r[pebble];
    int c = I_c[pebble];

    for (int i = 0; i <= M; i++) {
        if (i == 0) {
            // pass
        } else if (i % 5 == 0) {
            int N = (B + M/5 * F[pebble]) % 1000003;
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

    int x = r % G_h;
    int y = c % G_w;
    
    printf("%d %d\n", x, y);
}

void prompt_computer(Board *board, State *state, char pebble) {


    
}

void prompt_player(Board *board, State *state, char pebble) {
    while (1) {
        printf("Player %c> ", pebble);

        int row = -1;
        int col = -1;
        int assigned = scanf("%d %d", &row, &col);
        // TODO: Make it reprompt on partial string

        if (assigned != 2) {
            continue; // Reprompt
        }
        
        int status = set_node(board, row, col, pebble);
       
        if (status == 0) {
            // Success
            break;
        } else if (status == 1) {
            // OOB
            continue;
        } else {
            // Victory condition
            print_board(board);
            printf("Player %c wins\n", pebble);
            exit(0);
        }
    }
}


void start_game(State *state, Board *board, char p1type, char p2type) {

    while(1) {
        print_board(board);
        // should it encounter a victory condition on the board state
        
        if (p1type == 'c') {
            prompt_computer(board, state, 'O');
        } else {
            prompt_player(board, state, 'O');
        }
        print_board(board);

        if (p2type == 'c') {
            prompt_computer(board, state, 'X');
        } else {
            prompt_player(board, state, 'X');
        }
    }

    /* generate_move(state, board, 0, 0); // side effects on x and y */
    /* generate_move(state, board, 0, 1); // side effects on x and y */
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
        return 1;
    }

    if (strcmp(argv[1], "c") == 0) {
        printf("%s\n", "Computer 1");
        p1type = 'c';
    } else if (strcmp(argv[1], "h") == 0) {
        printf("%s\n", "Human 1");
        p1type = 'h';
    } else {
        fprintf(stderr, "%s\n", "Invalid player type");
        return 2;
    }

    if (strcmp(argv[2], "c") == 0) {
        printf("%s\n", "Computer 2");
        p2type = 'c';
    } else if (strcmp(argv[2], "h") == 0) {
        printf("%s\n", "Human 2");
        p2type = 'h';
    } else {
        fprintf(stderr, "%s\n", "Invalid player type");
        return 2;
    }

    // Board dimensions invalid checked first
    if (argc == 5) {
        // atoi returns 0 if conversion fails
        if (4 <= atoi(argv[3]) && atoi(argv[3]) <= 100) {
            height = atoi(argv[3]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            return 3;
        }

        if (4 <= atoi(argv[4]) && atoi(argv[4]) <= 100) {
            width = atoi(argv[4]);
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            return 3;
        }
    } else {
        printf("%s\n", "LOADING FILE");
        State *state = load_state(argv[3]);
        Board *board = load_board(argv[3]);

        start_game(state, board, p1type, p2type);
    } 
    
    State *state = new_State();
    Board *board = new_Board(height, width);  
    start_game(state, board, p1type, p2type);
         
    return 0;
}


