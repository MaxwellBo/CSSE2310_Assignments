#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "client.c"

#define READ_DESCRIPTOR 0
#define WRITE_DESCRIPTOR 1

#define NON_PLAYER_ARGS 3

#define MIN_PLAYERS 2
#define MAX_PLAYERS 26

#define ROLLFILE 1
#define WINSCORE 2

#define DICE 6

typedef struct State {
    FILE *rollfile;
    int winscore;
    int playerCount;
    Client **clients;
    Client *stLucia;
} State;

/**
 * Creates a new gamestate, after being supplied the number of players, the
 * score the players will play to, and a FILE pointing to a file containing
 * the rolls that the game should provide to the players when they roll
 *
 * - Allocates memory
 *
 * Returns a pointer to the newly allocated struct
 */
State *new_state(FILE *rollfile, int winscore, int playerCount) {
    State *self = malloc(sizeof(State));

    self->rollfile = rollfile;
    self->winscore = winscore;
    self->playerCount = playerCount;
    self->clients = malloc(sizeof(Client *) * playerCount);
    self->stLucia = NULL;

    return self;
}

/**
 * Takes the argument count and argument array, and checks if they're valid.
 *
 * - Terminates the program if the program if invalid arguments are provided,
 *   and prints an appropriate error message to stderr
 */
void validate_args(int argc, char **argv) {
    int status;

    if (!((3 + MIN_PLAYERS) <= argc && argc <= (3 + MAX_PLAYERS))) {
        status = 1;
    } else if (!(0 < atoi(argv[WINSCORE]))) {
        status = 2;
    } else {
        return;
    }

    fprintf(stderr, "%s\n", get_error_message_stlucia(status));
    exit(status);
}

/**
 * Takes a FILE pointer and verifies whether it can be used as a rollfile,
 * AKA, only containing valid rolls.
 *
 * - Terminates the program if the rollfile either doesn't exist, or has
 *   invalid contents, and prints an appropriate error message to stderr
 */
void validate_rollfile(FILE *rollfile) {

    if (rollfile == NULL) {
        fprintf(stderr, "%s\n", get_error_message_stlucia(3));
        exit(3);
    }

    while (1) {
        char c = fgetc(rollfile);

        if (c == EOF) { 
            rewind(rollfile);
            return;
        }

        if (!((c == '1') || (c == '2') || (c == '3') 
                || (c == 'H') || (c == 'A') || (c == 'P') || (c == '\n'))) {
            fprintf(stderr, "%s\n", get_error_message_stlucia(4));
            exit(4);
        }
    }
}

/**
 * Give a FILE pointer to a valid rollfile, gets the next roll from it
 *
 * - Mutates the position indicator for the stream
 */
char get_roll(FILE *rollfile) {
    while(1) {
        char roll = fgetc(rollfile);
        
        if (roll == '\n') {
            continue;
        } else if (roll == EOF) {
            rewind(rollfile);
        } else {
            return roll;
        }
    }
}

/*
 * Used as a compare function for a sorting algorithm, such that
 * '1' < '2' < '3' < 'H' < 'A' < 'P'
 *
 * - Pure
 *
 * Returns -1 if the first argument is less than the second, 0 if the same
 * and 1 if the first argument is greater than the second
 */
int compare_rolls(const void *a, const void *b) {

    char charA = *((char *)a);
    char charB = *((char *)b);

    // Treat 'A' as 'J', so that 'H' < 'A' < 'P' when sorted (as 'H' < 'J')
    charA = (charA == 'A') ? 'J' : charA;
    charB = (charB == 'A') ? 'J' : charB;

    if (charA < charB) {
        return -1;
    } else if (charA == charB) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * Makes a null terminated, memory allocated string filled with rolls pulled
 * from the specified rollfile. The rolls are ordered in ascending order
 * of value
 *
 * - Allocates memory
 * - Increments the position indicator for the stream
 *
 * Returns a pointer to the string
 */
char *get_rolls(FILE *rollfile) {
    char *collector = malloc(sizeof(char) * DICE + 1);
    collector[DICE] = '\0';

    for (int i = 0; i < DICE; i++) {
        collector[i] = get_roll(rollfile);
    }

    // Sort the first 6 elems of the collector, leaving '\0' in place
    qsort(collector, 6, sizeof(char), compare_rolls);

    return collector;
}

/**
 * Gets a null terminated string of rerolls (like "126"), and null 
 * terminated string of rolls (like "123HAB"), and rerolls the rolls
 * (1 indexed).
 *
 * - Allocates memory
 * -Increments the position indicator for the strema
 *
 * Returns a pointer to the string
 */
char *get_rerolls(FILE *rollfile, char *rolls, char *rerolls) {
    // Copy so we don't mutate the rolls
    char *collector = make_string(rolls);

    // Reroll the specified dice
    while (*rerolls != '\0') {
        int index = *rerolls - '1';
        collector[index] = get_roll(rollfile);
        rerolls++;
    }

    qsort(collector, 6, sizeof(char), compare_rolls);

    return collector;
}

/**
 * With a state, gets a '\n' terminated, null terminated string 
 * to be sent to all non-shutdown children.
 *
 * - Broadcasts to children
 */
void broadcast_all(State *self, char *message) {
    for (int i = 0; i < self->playerCount; i++) {
        if (!self->clients[i]->shutdown) {
            fprintf(self->clients[i]->pipe->outbox, "%s", message);
            fflush(self->clients[i]->pipe->outbox);
        }
    }
}

/**
 * With a state, gets a '\n' terminated, null terminated string 
 * to be sent to all non-shutdown children, with the exception of a
 * specified Client.
 *
 * - Broadcasts to children
 */
void broadcast_others(State *self, Client *exempt, char *message) {
    for (int i = 0; i < self->playerCount; i++) {
        if (self->clients[i] != exempt && !self->clients[i]->shutdown) {
            fprintf(self->clients[i]->pipe->outbox, "%s", message);
            fflush(self->clients[i]->pipe->outbox);
        }
    }
}

/**
 * Gets a Client and heals it, updating its model and printing a
 * notification to stderr.
 *
 * - Performs IO to stderr
 */
void heal(Client *patient, int health) {
    int oldHealth = patient->faculty->health;
    give_hs(patient->faculty, health);
    int newHealth = patient->faculty->health;
    int delta = newHealth - oldHealth;

    // Only display the message if we tried to heal
    if (health && !patient->faculty->inStLucia) {
        fprintf(stderr, "Player %c healed %d, health is now %d\n",
                patient->label, delta, newHealth);
    }
}

/**
 * Gets a Client and damages it, updating its model and printing a
 * notification to stderr.
 *
 * - Performs IO to stderr
 * - Mutates Client
 */
void attack(Client *attacked, int damage) {
    int oldHealth = attacked->faculty->health;
    give_as(attacked->faculty, damage);
    int newHealth = attacked->faculty->health;
    int delta = newHealth - oldHealth;

    fprintf(stderr, "Player %c took %d damage, health is now %d\n",
            attacked->label, -delta, newHealth);
}

/**
 * With a state, and a Client who is in St Lucia, and an amount of damage,
 * deals that amount of damage to all non-shutdown clients.
 *
 * - Broadcasts to children, mutating their models
 * - Mutates States, updating its model.
 */
void attack_out(State *self, Client *attacking, int damage) {
    char broadcastMsg[strlen("attacks p v outn0")];
    memset(broadcastMsg, 0, strlen("attacks p v outn0"));
    sprintf(broadcastMsg, "attacks %c %d out\n", attacking->label, damage);
    broadcast_all(self, broadcastMsg);

    for (int i = 0; i < self->playerCount; i++) {
        if (self->clients[i] != attacking && !self->clients[i]->shutdown) {
            attack(self->clients[i], damage);
        }
    }     
}

/**
 * With a State, and a Client who is not in St Lucia, and an amount of damage,
 * deals that amount of damage to the Client in St Lucia.
 *
 * - Broadcasts to children, mutating their models
 * - Mutates State, updating its model.
 */
void attack_in(State *self, Client *attacking, int damage) {
    char broadcastMsg[strlen("attacks p v inn0")];
    memset(broadcastMsg, 0, strlen("attacks p v inn0"));
    sprintf(broadcastMsg, "attacks %c %d in\n", attacking->label, damage);
    broadcast_all(self, broadcastMsg);

    attack(self->stLucia, damage);
}

/**
 * With a State, and a Client who's turn it is, and a tally array, and an
 * initial score to start from
 *
 * 1) Does the calculations for '1', '2' and '3's
 * 2) Deals with the Client's new tokens
 *
 *
 * - Performs IO to stderr, printing a notification of the players change
 *   of score
 * - Broadcasts to children, mutating their models
 * - Mutate State, updating its model
 */
void score_rolls(State *self, Client *currentPlayer, int *tallys, 
        int startValue) {
    int points = startValue;

    // if there are n 1s and n > 2. Gain n − 2 points
    if (tallys[0] > 2) {
        points += tallys[0] - 2;
    }

    // if there are n 2s and n > 2. Gain 2+(n−3) points
    if (tallys[1] > 2) {
        points += 2 + (tallys[1] - 3);
    }

    // if there are n 3s and n > 2. Gain 3 + (n − 3) points
    if (tallys[2] > 2) {
        points += 3 + (tallys[2] - 3);
    }

    // if there are n > 0 Ps, then gain n tokens. 
    // During their turn, if a player has 10 tokens saved up
    // they lose 10 tokens and gain 1 point.
    currentPlayer->faculty->tokens += tallys[5];

    if (currentPlayer->faculty->tokens >= 10) {
        currentPlayer->faculty->tokens -= 10;
        points += 1;
    }

    currentPlayer->faculty->score += points; 

    if (points) {
        char broadcastMsg[strlen("points c XXXXXn0")];
        sprintf(broadcastMsg, "points %c %d\n", currentPlayer->label, points);
        broadcast_all(self, broadcastMsg);

        fprintf(stderr, "Player %c scored %d for a total of %d\n", 
                currentPlayer->label, points, currentPlayer->faculty->score);
    }
}

/**
 * With a State, check for models that are dead, but haven't been told to
 * shutdown.
 * 
 * - Broadcasts to childen, telling them that one of their peers died,
 *   and shutting down the one that died
 * - Mutates the State, recording which Clients have been shutdown
 */
void process_eliminated(State *self) {
    for (int i = 0; i < self->playerCount; i++) {
        if (self->clients[i]->faculty->eliminated 
                && !self->clients[i]->shutdown) {
            char broadcastMsg[strlen("eliminated pn0")];
            memset(broadcastMsg, 0, strlen("eliminated pn0"));
            sprintf(broadcastMsg, "eliminated %c\n", self->clients[i]->label);

            broadcast_all(self, broadcastMsg);
            self->clients[i]->shutdown = true;
        }
    }     
}

/**
 * With a State, check for Clients that have won, when someone has
 * exceeded the winscore or is the last Client standing, and ends the game.
 *
 * - Broadcasts to children, telling them who won, and shutting them down
 * - Performs IO to stderr, with a notification of the end of the game'
 * - Terminates the program 
 */
void process_winner(State *self) {
    char label = '0';

    for (int i = 0; i < self->playerCount; i++) {
        // will end when at least one player has 15 or more points
        if (self->clients[i]->faculty->score >= self->winscore) {
            label = self->clients[i]->label;
            break;
        }
    }

    int aliveCount = 0;
    Client *standing;

    // If one person is alive, they win
    for (int i = 0; i < self->playerCount; i++) {
        // will end when at least one player has 15 or more points
        if (!self->clients[i]->faculty->eliminated) {
            standing = self->clients[i];
            aliveCount++;
        }
    }

    if (aliveCount == 1) {
        label = standing->label; 
    }
    
    if (label != '0') {
        char broadcastMsg[strlen("winner pn0")];
        memset(broadcastMsg, 0, strlen("winner pn0"));
        sprintf(broadcastMsg, "winner %c\n", label);
        broadcast_all(self, broadcastMsg);

        fprintf(stderr, "Player %c wins\n", label);
        exit(0);
    }
}

void process_end_of_turn(State *self, Client *currentPlayer, char *rolls) {

    int score = 0;
    // ---------- INFORM OTHER PLAYERS WHAT WAS ROLLED -----------
    char broadcastMsg[strlen("rolled p XXXXXXn0")];

    sprintf(broadcastMsg, "rolled %c %s\n", currentPlayer->label, rolls);
    broadcast_others(self, currentPlayer, broadcastMsg);

    fprintf(stderr, "Player %c rolled %s\n", currentPlayer->label, rolls);


    // ---------- HEALING ----------
    int *tallys = tally_faces(rolls);

    heal(currentPlayer, tallys[3]);

    // ---------- ATTACKS ARE PROCESSED AND DAMAGE REPORTED ----------

    if (self->stLucia == currentPlayer) {
        // Whenever a player starts their turn in StLucia, they gain 2 points
        score += 2;
    }

    // if there are n > 0 As
    if (tallys[4] > 0) {
        if (self->stLucia == currentPlayer) {
            // If the player is in StLucia, do n damage to all other players.
            attack_out(self, currentPlayer, tallys[4]);
        } else if (self->stLucia != NULL) {
            // If the player is not in StLucia, 
            // do n damage to the player in StLucia.
            attack_in(self, currentPlayer, tallys[4]);
            // Even if a player chooses to leave, they still take damage.

            fprintf(self->stLucia->pipe->outbox, "%s\n", "stay?");
            fflush(self->stLucia->pipe->outbox);
            char *line = read_line(self->stLucia->pipe->inbox);
            // fprintf(stderr, "From child:%s\n", line);

            if (!strcmp(line, "go")) {
                self->stLucia->faculty->inStLucia = false;
                self->stLucia = NULL;
            }

            free(line);
        } else {
            // If there is no player in StLucia,
            // claim StLucia (no damage is done in this case).
        }
    }

    // ---------- NEW PLAYER CLAIMS STLUCIA ----------
    if (self->stLucia == NULL && tallys[4]) {
        self->stLucia = currentPlayer;
        self->stLucia->faculty->inStLucia = true;

        // Whenever a player claims StLucia, they gain 1 point.
        score += 1;

        sprintf(broadcastMsg, "claim %c\n", currentPlayer->label);
        broadcast_all(self, broadcastMsg);

        fprintf(stderr, "Player %c claimed StLucia\n", currentPlayer->label);
    }

    // ---------- POINTS FOR THE TURN ARE REPORTED ----------
    score_rolls(self, currentPlayer, tallys, score);


    // ---------- PLAYER ELIMINATIONS ARE REPORTED ----------
    process_eliminated(self);

    // ---------- TEST FOR GAME OVER ----------
    process_winner(self);
}

// IMPURE
void main_loop(State *self) {

    while (1) {
        for (int i = 0; i < self->playerCount; i++) {
            if (!self->clients[i]->shutdown) {
                // Starting rolls
                char *rolls = get_rolls(self->rollfile);
                fprintf(self->clients[i]->pipe->outbox, "turn %s\n", rolls);
                fflush(self->clients[i]->pipe->outbox);

                while (1) {
                    char *input = read_line(self->clients[i]->pipe->inbox);
                    // fprintf(stderr, "From child %c:%s\n",
                    // self->clients[i]->label, input);

                    char *line = input;
                    if (input[0] == '!') {
                        line = (input + 1);
                    }

                    // Max length
                    char command[strlen("eliminated0")];
                    sscanf(line, "%s ", command);
                    // For dealing with useless spec adjustments
                    sscanf(line, "!%s ", command);

                    // 0 on successful compare
                    if (!strcmp(command, "reroll")) {
                        rolls = get_rerolls(self->rollfile, rolls,
                            &line[strlen("reroll ")]);
                        fprintf(self->clients[i]->pipe->outbox, 
                            "rerolled %s\n", rolls);
                        fflush(self->clients[i]->pipe->outbox);
                    } else if (!strcmp(command, "keepall")) {
                        process_end_of_turn(self, self->clients[i], rolls);
                        break;
                    } else {
                        fprintf(stderr, "%s\n", get_error_message_stlucia(7));
                        // exit(7);
                    }

                    free(input);
                }

                free(rolls);
            }
        }
    }
}

/*
 * Author: 43926871
 *
 * - Performs IO to stdout and stderr
 * - Can terminate the program
 */
int main(int argc, char **argv) {
    // ---------- VALIDATION ---------- 
    // Can terminate the program 
    validate_args(argc, argv);

    FILE *rollfile = fopen(argv[ROLLFILE], "r");

    // Can terminate the program
    validate_rollfile(rollfile);

    // ---------- MODEL CREATION ---------- 
    int playerCount = argc - NON_PLAYER_ARGS;

    State *state = new_state(rollfile, atoi(argv[WINSCORE]), playerCount);

    // ---------- FORKING ---------- 
    for (int i = 0; i < playerCount; i++) {

        // A for (i = 0), B for (i = 1)
        char label[2] = {(char)i + 'A', '\0'};
        state->clients[i] = new_client(label[0]);

        pid_t pid = fork();
        state->clients[i]->pid = pid;

        if (pid == 0) {
            // ---------- CHILD ---------- 
            // Redirect stdin/stdout
            use_as_child(state->clients[i]->pipe);

            char playerCountArg[3];
            sprintf(playerCountArg, "%d", playerCount);
            playerCountArg[2] = '\0';

            execl(argv[NON_PLAYER_ARGS + i], "player", playerCountArg, label, NULL);
        } else {
            // ---------- PARENT ---------- 
            // Open file pointers to the child processes
            use_as_parent(state->clients[i]->pipe);
        }
    }

    // ---------- PARENT ---------- 
    main_loop(state);
}

