#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.c"
#include "state.c"

void debug(void);
void start_game(State *state, Board *board);

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
            printf("%s\n", "Assigning height");
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            return 3;
        }

        if (4 <= atoi(argv[3]) && atoi(argv[3]) <= 100) {
            printf("%s\n", "Assigning width");
        } else {
            fprintf(stderr, "%s\n", "Invalid board dimension");
            return 3;
        }
    } else {
        printf("%s\n", "LOADING FILE");
        State *state = load_state(argv[3]);
        Board *board = load_board(argv[3]);

        start_game(state, board);
    } 
    
    State *state = new_State();
    Board *board = new_Board(height, width);  
    start_game(state, board);
         
    return 0;
}

void start_game(State *state, Board *board) {
    print_board(board);
}

// 0 for "O", 1 for "X"
/* void computer_move(int *x, int *y, int pebble_type, int move_count) { */
/*     // 0th for "0", 1th for "X" */
/*     int I_r[] = { 1, 2 }; */
/*     int I_c[] = { 4, 10 }; */
/*     int F[] = { 29, 17 }; */
/* } */
