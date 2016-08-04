#ifndef STATE_H
#define STATE_H

typedef struct State {
    int next_player;
    int row_for_O;
    int col_for_O;
    int move_number_for_O;
    int row_for_X;
    int col_for_X;
    int move_number_for_X;
} State;


State *new_State(void);
State *load_state(char *filename);
void free_state(State *self);

#endif
