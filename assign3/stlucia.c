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


State *new_state(FILE *rollfile, int winscore, int playerCount) {
    State *self = malloc(sizeof(State));

    self->rollfile = rollfile;
    self->winscore = winscore;
    self->playerCount = playerCount;
    self->clients = malloc(sizeof(Client *) * playerCount);
    self->stLucia = NULL;

    return self;
}

// IMPURE
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

// IMPURE
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

// IMPURE
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

// IMPURE
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


// Prereq, must be \n terminated
void broadcastAll(State *self, char *message) {
    for (int i = 0; i < self->playerCount; i++) {
        if (!self->clients[i]->shutdown) {
            fprintf(self->clients[i]->pipe->outbox, "%s", message);
            fflush(self->clients[i]->pipe->outbox);
        }
    }
}

void broadcastOthers(State *self, Client *exempt, char *message) {
    for (int i = 0; i < self->playerCount; i++) {
        if (self->clients[i] != exempt && !self->clients[i]->shutdown) {
            fprintf(self->clients[i]->pipe->outbox, "%s", message);
            fflush(self->clients[i]->pipe->outbox);
        }
    }
}


void heal(Client *patient, int health) {
     int oldHealth = patient->faculty->health;
     give_Hs(patient->faculty, health);
     int newHealth = patient->faculty->health;
     int delta = newHealth - oldHealth;

     // Only display the message if we tried to heal
     if (health) {
        fprintf(stderr, "Player %c healed %d, health is now %d\n",
            patient->label, delta, newHealth);
     }
}

void attack(Client *attacked, int damage) {
    int oldHealth = attacked->faculty->health;
    give_As(attacked->faculty, damage);
    int newHealth = attacked->faculty->health;
    int delta = newHealth - oldHealth;

    fprintf(stderr, "Player %c took %d damage, health is now %d\n",
        attacked->label, -delta, newHealth);
}

void attackOut(State *self, Client *attacking, int damage) {
    char broadcastMsg[strlen("attacks p v outn0")];
    memset(broadcastMsg, 0, strlen("attacks p v outn0"));
    sprintf(broadcastMsg, "attacks %c %d out\n", attacking->label, damage);
    broadcastAll(self, broadcastMsg);

    for (int i = 0; i < self->playerCount; i++) {
        if (self->clients[i] != attacking) {
            attack(self->clients[i], damage);
        }
    }     
}

void attackIn(State *self, Client *attacking, int damage) {
    char broadcastMsg[strlen("attacks p v inn0")];
    memset(broadcastMsg, 0, strlen("attacks p v inn0"));
    sprintf(broadcastMsg, "attacks %c %d in\n", attacking->label, damage);
    broadcastAll(self, broadcastMsg);

    attack(self->stLucia, damage);
}

void score_rolls(State *self, Client *currentPlayer, int *tallys, int startValue) {
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
    // During their turn, if a player has 10 tokens saved up, they lose 10 tokens and gain 1 point.
    currentPlayer->faculty->tokens += tallys[5];

    if (currentPlayer->faculty->tokens > 10) {
        currentPlayer->faculty->tokens -= 10;
        points += 1;
    }

    currentPlayer->faculty->score += points; 

    if (points) {
       char broadcastMsg[strlen("points c XXXXXn0")];
       sprintf(broadcastMsg, "points %c %d\n", currentPlayer->label, points);
       broadcastAll(self, broadcastMsg);

       fprintf(stderr, "Player %c scored %d for a total of %d\n", 
           currentPlayer->label, points, currentPlayer->faculty->score);
    }
}

void process_eliminated(State *self) {
    for (int i = 0; i < self->playerCount; i++) {
        if (self->clients[i]->faculty->eliminated) {
           char broadcastMsg[strlen("eliminated pn0")];
            memset(broadcastMsg, 0, strlen("eliminated pn0"));
           sprintf(broadcastMsg, "eliminated %c\n", self->clients[i]->label);

           broadcastAll(self, broadcastMsg);
           self->clients[i]->shutdown = true;
        }
    }     
}

void process_winner(State *self) {
    for (int i = 0; i < self->playerCount; i++) {
        // will end when at least one player has 15 or more points
        if (self->clients[i]->faculty->score >= self->winscore) {

            char broadcastMsg[strlen("winner pn0")];
            memset(broadcastMsg, 0, strlen("winner pn0"));
            sprintf(broadcastMsg, "winner %c\n", self->clients[i]->label);
            broadcastAll(self, broadcastMsg);

            fprintf(stderr, "Player %c wins\n", self->clients[i]->label);
            exit(0);
        }
    }

}

void process_end_of_turn(State *self, Client *currentPlayer, char *rolls) {

    int score = 0;
    // ---------- INFORM OTHER PLAYERS WHAT WAS ROLLED -----------
    char broadcastMsg[strlen("rolled p XXXXXXn0")];

    sprintf(broadcastMsg, "rolled %c %s\n", currentPlayer->label, rolls);
    broadcastOthers(self, currentPlayer, broadcastMsg);

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
            attackOut(self, currentPlayer, tallys[4]);
        } else if (self->stLucia != NULL) {
            // If the player is not in StLucia, do n damage to the player in StLucia.
            attackIn(self, currentPlayer, tallys[4]);
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
            // If there is no player in StLucia, claim StLucia (no damage is done in this case).
        }
    }

    // ---------- NEW PLAYER CLAIMS STLUCIA ----------
    if (self->stLucia == NULL && tallys[4]) {
        self->stLucia = currentPlayer;
        self->stLucia->faculty->inStLucia = true;

        // Whenever a player claims StLucia, they gain 1 point.
        score += 1;

        sprintf(broadcastMsg, "claim %c\n", currentPlayer->label);
        broadcastAll(self, broadcastMsg);

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
                    // fprintf(stderr, "From child %c:%s\n", self->clients[i]->label, input);

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
                        rolls = get_rerolls(self->rollfile, rolls, &line[strlen("reroll ")]);
                        fprintf(self->clients[i]->pipe->outbox, "rerolled %s\n", rolls);
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
        char label[2] = { (char)i + 'A', '\0' };
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

