#include <stdlib.h>                                                   /* QS: */
#include <stdbool.h>                                                  /* QS: */
#include <string.h>                                                   /* QS: */
#include <ctype.h>                                                    /* QS: */
#include <stdio.h>                                                    /* QS: */
#include "extra.h"                                                    /* QS: */
                                                                      /* QS: */
typedef struct {                                                      /* QS: */
    unsigned int r, c;                                                /* QS: */
    unsigned int h, w;                                                /* QS: */
    unsigned long movesnum;                                           /* QS: */
    unsigned int mult;                                                /* QS: */
    unsigned int basevalue;                                           /* QS: */
} MoveFn;                                                             /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
void init_movefn(MoveFn* f, unsigned int h, unsigned w, unsigned int r,/* QS: */
 unsigned c, unsigned long movesnum, unsigned int mult);              /* QS: */
void get_move(MoveFn* f, unsigned int* r, unsigned int* c);           /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
typedef enum {                                                        /* QS: */
    OK=0,                                                        /* QS:a1j7e */
    USAGE=1,                                                     /* QS:a2m3e */
    PTYPE=2,                                                     /* QS:a3p9d */
    DIM=3,                                                       /* QS:a4s5d */
    LOADOPEN=4,                                                  /* QS:a5v1d */
    LOADPROC=5,                                                  /* QS:a6y7c */
    EOINPUT=6,                                                   /* QS:a7b4c */
    SAVE=7                                                       /* QS:a8e0c */
} ERR;                                                                /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
ERR show_err(ERR e);                                                  /* QS: */
                                                                      /* QS: */
typedef struct Game                                                   /* QS: */
{                                                                     /* QS: */
    char** data;                                                 /* QS:a9h6b */
    unsigned int height;                                              /* QS: */
    unsigned int width;                                               /* QS: */
    bool turn;                                                        /* QS: */
    bool types[2];      // true for human                             /* QS: */
    char chars[2];                                               /* QS:b0k2b */
    bool winner;                                                      /* QS: */
    MoveFn cmoves[2];                                            /* QS:b1n8a */
} Game;                                                               /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
Game* createGame(unsigned int h, unsigned int w);                     /* QS: */
void freeGame(Game* g);                                               /* QS: */
void print_game(FILE* f, Game* g);                                    /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
bool hasLiberties(Game* g, unsigned int r, unsigned int c);           /* QS: */
ERR play_game(Game* g);                                               /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
void save_game(FILE* f, Game* g);                                     /* QS: */
Game* load_game(FILE* f);                                             /* QS: */
#define BLANK '.'                                                /* QS:b2q4a */
#define P1C 'O'                                                  /* QS:b3t0a */
#define P2C 'X'                                                  /* QS:b3w6z */
                                                                      /* QS: */
                                                                      /* QS: */
typedef struct Stack {                                                /* QS: */
    unsigned int* values;                                        /* QS:b4z2z */
    unsigned int cap;                                                 /* QS: */
    unsigned int size;                                                /* QS: */
} Stack;                                                              /* QS: */
                                                                      /* QS: */
Stack* allocStack(void);                                              /* QS: */
void freeStack(Stack* s);                                             /* QS: */
void push(Stack* s, unsigned int r,unsigned int c);                   /* QS: */
bool pop(Stack* s, unsigned int* r, unsigned int* c);                 /* QS: */
                                                                      /* QS: */
ERR show_err(ERR e) {                                                 /* QS: */
    const char* estring="";                                           /* QS: */
    switch (e) {                                                      /* QS: */
    case OK: break;                                                   /* QS: */
    case USAGE: estring="Usage: nogo p1type p2type [height width |"   /* QS: */
" filename]\n"; break;                                           /* QS:b5c9y */
    case PTYPE: estring="Invalid player type \n"; break;         /* QS:b6f5y */
    case DIM:   estring="Invalid board dimension\n"; break;      /* QS:b7i1y */
    case LOADOPEN:estring="Unable to open file\n"; break;        /* QS:b8l7x */
    case LOADPROC:estring="Incorrect file contents\n";break;     /* QS:b9o3x */
    case SAVE:estring="Unable to save game\n"; break;            /* QS:c0r9w */
    case EOINPUT:estring="End of input from user\n"; break;      /* QS:c1u5w */
    };                                                                /* QS: */
    fprintf(stderr, "%s", estring);                              /* QS:c2x1w */
    printf("%d: %s", e, estring);
    return e;                                                         /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void init_movefn(MoveFn* f, unsigned int h, unsigned w, unsigned int r,/* QS: */
 unsigned c, unsigned long movesnum, unsigned int mult);              /* QS: */
                                                                      /* QS: */
Game* createGame(unsigned int h, unsigned int w)                      /* QS: */
{                                                                     /* QS: */
    Game* res=malloc(sizeof(Game));                                   /* QS: */
    res->data=malloc(sizeof(char*)*h);                           /* QS:c3a8v */
    for (unsigned int i=0;i<h;++i)                               /* QS:c4d4v */
    {                                                                 /* QS: */
        res->data[i]=malloc(sizeof(char)*(w+1));// allow space for \0 /* QS: */
        memset(res->data[i], '.', w);   // fill with blanks           /* QS: */
        res->data[i][w]='\0';           // ensure always terminated   /* QS: */
    }                                                                 /* QS: */
    res->height=h;                                               /* QS:c5g0v */
    res->width=w;                                                /* QS:c6j6u */
    res->turn=0;                                                 /* QS:c7m2u */
    res->chars[0]=P1C;                                           /* QS:c8p8t */
    res->chars[1]=P2C;                                           /* QS:c9s4t */
    init_movefn(&res->cmoves[0], h,w, 1, 4, 0, 29);              /* QS:d0v0t */
    init_movefn(&res->cmoves[1], h,w, 2, 10, 0, 17);             /* QS:d1y6s */
    return res;                                                       /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void freeGame(Game* g)                                                /* QS: */
{                                                                     /* QS: */
    if (g!=0)                                                         /* QS: */
    {                                                                 /* QS: */
        for (unsigned int i=0;i<g->height;++i)                        /* QS: */
        {                                                             /* QS: */
            free(g->data[i]);                                    /* QS:d2b3s */
        }                                                             /* QS: */
        free(g->data);                                           /* QS:d3e9r */
        free(g);                                                      /* QS: */
    }                                                                 /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void add_neighbours(Game* g, Stack* s, unsigned int r, unsigned int c,/* QS: */
 char seen) {                                                         /* QS: */
    if ((r>0) && (g->data[r-1][c]!=seen)) {                      /* QS:d4h5r */
            push(s, r-1, c);                                     /* QS:d5k1r */
    }                                                                 /* QS: */
    if ((r<g->height-1) && (g->data[r+1][c]!=seen)) {            /* QS:d6n7q */
            push(s, r+1, c);                                     /* QS:d7q3q */
    }                                                                 /* QS: */
    if ((c>0) && (g->data[r][c-1]!=seen)) {                      /* QS:d8t9p */
            push(s, r, c);                                       /* QS:d9w5p */
    }                                                                 /* QS: */
    if ((c<g->width-1) && (g->data[r][c+1]!=seen)) {             /* QS:e0z1p */
            push(s, r, c+1);                                     /* QS:e1c8o */
    }                                                                 /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
bool do_search(Game* g, Stack* search, Stack* revert, char us,        /* QS: */
char us_seen) {                                                       /* QS: */
    unsigned int r, c;                                                /* QS: */
    while (pop(search, &r, &c)) {                                     /* QS: */
        char v=g->data[r][c];                                    /* QS:e2f4o */
        if (v==BLANK) {                                               /* QS: */
            return true;                                              /* QS: */
        }                                                             /* QS: */
        if (v==us_seen) {                                        /* QS:e3i0o */
            continue;           // try next one                       /* QS: */
        }                                                             /* QS: */
        if (v==us) {    // add neighbours                             /* QS: */
            g->data[r][c]=us_seen;                               /* QS:e4l6n */
            push(revert, r, c);                                       /* QS: */
            add_neighbours(g, search, r, c, us_seen);                 /* QS: */
        }                                                             /* QS: */
    }                                                                 /* QS: */
    return false;// nowhere else to go and we haven't found a liberty /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
bool hasLiberties(Game* g, unsigned int r, unsigned int c)            /* QS: */
{                                                                     /* QS: */
      if ((r>=g->height) || (c>=g->width)) {                     /* QS:e5o2n */
          return false;                                          /* QS:e6r8m */
      }                                                               /* QS: */
      if (g->data[r][c]==BLANK) {                                /* QS:e7u4m */
          return true;                                           /* QS:e8x0m */
      }                                                               /* QS: */
      Stack* search=allocStack();                                     /* QS: */
      Stack* revert=allocStack();                                /* QS:e9a7l */
      char us=g->data[r][c];                                          /* QS: */
      char us_seen=tolower(us);                                       /* QS: */
        // flip this cell to searched                                 /* QS: */
      push(revert, r, c);                                             /* QS: */
        // push our neighbours                                        /* QS: */
      add_neighbours(g, search, r, c, us_seen);                       /* QS: */
      bool res=do_search(g, search, revert, us, us_seen);             /* QS: */
      freeStack(search);                                              /* QS: */
      unsigned int fr, fc;                                            /* QS: */
      while (pop(revert, &fr, &fc)) {                                 /* QS: */
          g->data[fr][fc]=us;                                    /* QS:f0d3l */
      }                                                               /* QS: */
      freeStack(revert);                                              /* QS: */
      return res;                                                     /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void print_game(FILE* f, Game* g) {                                   /* QS: */
    fputc('/', f);                                               /* QS:f1g9k */
    for (unsigned int i=0;i<g->width;++i) {                      /* QS:f2j5k */
        fputc('-', f);                                           /* QS:f3m1k */
    }                                                                 /* QS: */
    fputc('\\', f);                                                   /* QS: */
    fputc('\n', f);                                              /* QS:f4p7j */
        // because I have \0 on the end of my arrays,                 /* QS: */
        // I can just treat them as strings                           /* QS: */
    for (unsigned int i=0;i<g->height;++i) {                     /* QS:f5s3j */
        fprintf(f, "|%s|\n", g->data[i]);                        /* QS:f6v9i */
    }                                                                 /* QS: */
    fputc('\\', f);                                              /* QS:f7y5i */
    for (unsigned int i=0;i<g->width;++i) {                      /* QS:f8b2i */
        fputc('-', f);                                                /* QS: */
    }                                                                 /* QS: */
    fputc('/', f);                                                    /* QS: */
    fputc('\n', f);                                              /* QS:f9e8h */
    fflush(f);                                                        /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void gobble_eoln(FILE* f) {                                           /* QS: */
    int in;                                                           /* QS: */
    while (in=fgetc(f), (in!=EOF) && (in!='\n')) {               /* QS:g0h4h */
    }                                                                 /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
bool get_input(Game* g, unsigned* r, unsigned* c) {                   /* QS: */
    char buffer[80];                                                  /* QS: */
    char dummy;                                                       /* QS: */
    do {                                                              /* QS: */
        printf("Player %c> ", g->chars[g->turn]);                /* QS:g1k0h */
        fflush(stdout);                                               /* QS: */
        if (!fgets(buffer, 20, stdin)) {        // END of file        /* QS: */
            return false;                                        /* QS:g2n6g */
        }                                                             /* QS: */
        if (buffer[0]=='\0') {                                   /* QS:g3q2g */
            continue;                                                 /* QS: */
        }                                                             /* QS: */
        if (buffer[0]=='w') {   // save                               /* QS: */
            if (buffer[strlen(buffer)-1]=='\n') {                     /* QS: */
              buffer[strlen(buffer)]='\0'; /*strip trailing \n*/ /* QS:g4t8f */
            }                                                         /* QS: */
            FILE* f=fopen(buffer+1,"w");                              /* QS: */
            if (f==0) {                                               /* QS: */
             fprintf(stderr,"Could not open file for writing\n");/* QS:g5w4f */
            } else {                                                  /* QS: */
             save_game(f, g);                                         /* QS: */
            }                                                         /* QS: */
        }                                                             /* QS: */
        else                                                          /* QS: */
        {                                                             /* QS: */
            if (buffer[strlen(buffer)-1]!='\n') {// remove extra text /* QS: */
                gobble_eoln(stdin);                                   /* QS: */
            }                                                         /* QS: */
            if ((sscanf(buffer, "%u%c%u", r,&dummy, c)<3)             /* QS: */
                || (dummy!=' ')) {                                    /* QS: */
                continue;                                             /* QS: */
            }                                                         /* QS: */
            if ((*r>=g->height) ||                               /* QS:g6z0f */
              (*c>=g->width)) { // out of bounds                 /* QS:g7c7e */
                continue;                                             /* QS: */
            }                                                         /* QS: */
            if (g->data[*r][*c]!=BLANK) {       // not empty     /* QS:g8f3e */
                continue;                                             /* QS: */
            }                                                         /* QS: */
            return true;                                              /* QS: */
        }                                                             /* QS: */
    } while (true);                                                   /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
bool check(Game* g, unsigned r, unsigned c) {                         /* QS: */
      // for each neighbour, check if they belong to our oponent      /* QS: */
      // and if they are eliminated                                   /* QS: */
      // Up                                                           /* QS: */
      // opponent                                                     /* QS: */
    if ((r>0) && (g->data[r-1][c]==g->chars[!g->turn])) {             /* QS: */
        if (!hasLiberties(g, r-1, c)) {                               /* QS: */
            g->winner=g->turn;                                   /* QS:g9i9d */
            return true;                                              /* QS: */
        }                                                             /* QS: */
    }                                                                 /* QS: */
      // Down                                                         /* QS: */
    if ((r<g->height-1) &&                                            /* QS: */
         (g->data[r+1][c]==g->chars[!g->turn])) {     // opponent     /* QS: */
        if (!hasLiberties(g, r+1, c)) {                          /* QS:h0l5d */
            g->winner=g->turn;                                   /* QS:h1o1d */
            return true;                                              /* QS: */
        }                                                             /* QS: */
    }                                                                 /* QS: */
      // left                                                         /* QS: */
      // opponent                                                     /* QS: */
    if ((c>0) && (g->data[r][c-1]==g->chars[!g->turn])) {             /* QS: */
        if (!hasLiberties(g, r, c-1)) {                          /* QS:h2r7c */
            g->winner=g->turn;                                   /* QS:h3u3c */
            return true;                                              /* QS: */
        }                                                             /* QS: */
    }                                                                 /* QS: */
      // right                                                        /* QS: */
      // opponent                                                     /* QS: */
    if ((c<g->width-1)                                                /* QS: */
      && (g->data[r][c+1]==g->chars[!g->turn])) {                     /* QS: */
        if (!hasLiberties(g, r, c+1)) {                          /* QS:h4x9b */
            g->winner=g->turn;                                   /* QS:h5a6b */
            return true;                                              /* QS: */
        }                                                             /* QS: */
    }                                                                 /* QS: */
      // none of the neighbours result in opponent being eliminated   /* QS: */
    if (!hasLiberties(g, r, c)) {                                     /* QS: */
        g->winner=!g->turn;                                      /* QS:h6d2b */
        return true;                                                  /* QS: */
    }                                                                 /* QS: */
    return false;                                                     /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
ERR play_game(Game* g) {                                              /* QS: */
    bool gameover=false;                                              /* QS: */
    print_game(stdout, g);                                            /* QS: */
    while (!gameover) {                                               /* QS: */
                                                                      /* QS: */
        unsigned int r, c;                                            /* QS: */
        if (g->types[g->turn])                                        /* QS: */
        {                                                             /* QS: */
            if (!get_input(g, &r, &c)) {                              /* QS: */
                return EOINPUT;                                       /* QS: */
            }                                                         /* QS: */
        } else {                                                      /* QS: */
            do {                                                      /* QS: */
                get_move(& g->cmoves[g->turn], &r, &c);               /* QS: */
            } while (g->data[r][c]==BLANK);                      /* QS:h7g8a */
            printf("Player %c: %u %d\n", g->chars[g->turn],r,c); /* QS:h8j4a */
        }                                                             /* QS: */
        g->data[r][c]=g->chars[g->turn];                         /* QS:h9m0a */
        gameover=check(g,r,c);                                   /* QS:h9p6z */
        g->turn=!g->turn;                                        /* QS:i0s2z */
        print_game(stdout, g);                                        /* QS: */
    }                                                                 /* QS: */
    printf("Player %c wins\n", g->chars[g->winner]);             /* QS:i1v8y */
    return OK;                                                        /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void save_game(FILE* f, Game* g) {                                    /* QS: */
    fprintf(f, "%u %u %u ", g->height, g->width, (unsigned)g->turn);  /* QS: */
    unsigned r=g->cmoves[0].r;                                   /* QS:i2y4y */
    unsigned c=g->cmoves[0].c;                                   /* QS:i3b1y */
      // now to save the values to reconstruct the move functions     /* QS: */
    fprintf(f, "%u %u %lu ", r, c, g->cmoves[0].movesnum);       /* QS:i4e7x */
    r=g->cmoves[1].r;                                            /* QS:i5h3x */
    c=g->cmoves[1].c;                                            /* QS:i6k9w */
    fprintf(f, "%u %u %lu\n", r, c, g->cmoves[1].movesnum);      /* QS:i7n5w */
      // now to dump the board                                        /* QS: */
    for (unsigned i=0;i<g->height;++i)                                /* QS: */
      fprintf(f, "%s\n",g->data[i]);                                  /* QS: */
    fclose(f);                                                        /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
Game* load_game(FILE* f) {                                            /* QS: */
    char buffer[50];            // bigger than we need                /* QS: */
    if (!fgets(buffer,50,f)) {                                        /* QS: */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    if ((strlen(buffer)==0) || (buffer[strlen(buffer)-1]!='\n')) {    /* QS: */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    int h, w, r1, c1, r2, c2;                                         /* QS: */
    char turnchar;                                                    /* QS: */
    int long m1, m2;                                                  /* QS: */
    if (sscanf(buffer, "%d %d %c %d %d %ld %d %d %ld",                /* QS: */
 &h, &w, &turnchar, &r1, &c1, &m1, &r2, &c2, &m2)!=9) {          /* QS:i8q1w */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    if ((h<4) || (w<4)) {                                             /* QS: */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    if ((turnchar!='0') &&                                            /* QS: */
      (turnchar!='1')) {                                         /* QS:i9t7v */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    if ((r1<0) || (c1<0) ||                                           /* QS: */
      (m1<0) || (r2<0) ||                                        /* QS:j0w3v */
      (c2<0) || (m2<0)) {                                        /* QS:j1z9u */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    if ((r1>=h) || (r2>=h) ||                                         /* QS: */
      (c1>=w) || (c2>=w)) {                                      /* QS:j2c6u */
        return 0;                                                     /* QS: */
    }                                                                 /* QS: */
    Game* g=createGame(h,w);                                          /* QS: */
    g->turn=turnchar=='1';                                       /* QS:j3f2u */
      // that's a blank board now to restore the move functions       /* QS: */
    g->cmoves[0].r=r1;                                           /* QS:j4i8t */
    g->cmoves[0].c=c1;                                                /* QS: */
    g->cmoves[0].movesnum=m1;                                         /* QS: */
    g->cmoves[1].r=r2;                                           /* QS:j5l4t */
    g->cmoves[1].c=c2;                                           /* QS:j6o0t */
    g->cmoves[1].movesnum=m2;                                         /* QS: */
      // now to load the board                                        /* QS: */
    for (unsigned int i=0;i<h;++i) {                                  /* QS: */
        int in;                                                       /* QS: */
        for (unsigned int j=0;j<w;j++) {                              /* QS: */
            in=fgetc(f);                                              /* QS: */
            if ((in==EOF) || ((in!=BLANK) && (in!=P1C) && (in!=P2C))){/* QS: */
                freeGame(g);                                          /* QS: */
                return 0;                                             /* QS: */
            }                                                         /* QS: */
            g->data[i][j]=(char)in;                              /* QS:j7r6s */
        }                                                             /* QS: */
        in=fgetc(f);                                                  /* QS: */
        if (in!='\n') {                                               /* QS: */
            freeGame(g);                                              /* QS: */
            return 0;                                                 /* QS: */
        }                                                             /* QS: */
    }                                                                 /* QS: */
    return g;                                                         /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
                                                                      /* QS: */
int main(int argc, char** argv) {                                     /* QS: */
    if ((argc!=4) && (argc!=5)) {                                     /* QS: */
        return show_err(USAGE);                                       /* QS: */
    }                                                                 /* QS: */
    printf("%d: %s %s %s %s ", argc, argv[0], argv[1], argv[2], argv[3]);

    int val = (argc == 5) ? printf("%s\n", argv[4]) : printf("\n"); 

    bool ptype[2]={true, true};                                       /* QS: */
    if (((argv[1][0]!='c') && (argv[1][0]!='h')) ||                   /* QS: */
         ((argv[2][0]!='c') && (argv[2][0]!='h')))                    /* QS: */
    {                                                                 /* QS: */
        return show_err(PTYPE);                                  /* QS:j8u2s */
    }                                                                 /* QS: */
    if ((strlen(argv[1])!=1) || (strlen(argv[2])!=1)) {               /* QS: */
        return show_err(PTYPE);                                  /* QS:j9x8r */
    }                                                                 /* QS: */
    ptype[0]=(argv[1][0]=='h');                                       /* QS: */
    ptype[1]=(argv[2][0]=='h');                                       /* QS: */
                                                                      /* QS: */
    if (argc==4) {      // load game                                  /* QS: */
        FILE* f=fopen(argv[3],"r");                                   /* QS: */
        if (!f) {                                                     /* QS: */
            return show_err(LOADOPEN);                           /* QS:k0a5r */
        }                                                             /* QS: */
        Game* g=load_game(f);                                         /* QS: */
        if (!g) {                                                     /* QS: */
            return show_err(LOADPROC);                                /* QS: */
        }                                                             /* QS: */
        g->types[0]=ptype[0];                                    /* QS:k1d1r */
        g->types[1]=ptype[1];                                    /* QS:k2g7q */
        ERR res=play_game(g);                                         /* QS: */
        freeGame(g);                                                  /* QS: */
        return show_err(res);                                         /* QS: */
                                                                      /* QS: */
    } else {    // new game                                           /* QS: */
        char* herr=0;                                                 /* QS: */
        unsigned int height=(unsigned int)strtoul(argv[3], &herr, 10);/* QS: */
        char* werr=0;                                                 /* QS: */
        unsigned int width=(unsigned int)strtoul(argv[4], &werr, 10); /* QS: */
        if ((*herr!='\0') || (*werr!='\0') ||                         /* QS: */
 (width>1000) || (height>1000) || (height<4) || (width<4)) {          /* QS: */
            return show_err(DIM);                                /* QS:k3j3q */
        }                                                             /* QS: */
        Game* g=createGame(height, width);                            /* QS: */
        g->types[0]=ptype[0];                                         /* QS: */
        g->types[1]=ptype[1];                                         /* QS: */
        ERR res=play_game(g);                                         /* QS: */
        freeGame(g);                                             /* QS:k4m9p */
        return show_err(res);                                         /* QS: */
    }                                                                 /* QS: */
    return 0;                                                         /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void init_movefn(MoveFn* f, unsigned int h, unsigned w, unsigned int r,/* QS: */
 unsigned c, unsigned long movesnum, unsigned int mult) {             /* QS: */
    f->basevalue=r*w+c;                                          /* QS:k5p5p */
    f->r=r;                                                      /* QS:k6s1p */
    f->c=c;                                                      /* QS:k7v7o */
    f->h=h;                                                      /* QS:k8y3o */
    f->w=w;                                                      /* QS:k9b0o */
    f->r=r%h;                                                    /* QS:l0e6n */
    f->c=c%w;                                                    /* QS:l1h2n */
    f->mult=mult;                                                /* QS:l2k8m */
    f->movesnum=movesnum;                                        /* QS:l3n4m */
}                                                                     /* QS: */
                                                                      /* QS: */
void get_move(MoveFn* f, unsigned int* r, unsigned int* c) {          /* QS: */
    unsigned int rdelta[5]={0,1,2,1,0};                               /* QS: */
    unsigned int cdelta[5]={0,1,1,0,1};                               /* QS: */
    *r=f->r % f->h;                                              /* QS:l4q0m */
    *c=f->c % f->w;                                              /* QS:l5t6l */
    f->movesnum++;                                               /* QS:l6w2l */
      // now we need to update for the next move                      /* QS: */
    if (f->movesnum%5==0) {     // time to generate a new base        /* QS: */
        unsigned long cycle=f->movesnum/5;                            /* QS: */
        unsigned long newval=(f->basevalue+cycle*f->mult) % 1000003;  /* QS: */
        f->r=newval/f->w;                                        /* QS:l7z8k */
        f->c=newval%f->w;                                        /* QS:l8c5k */
    } else {                                                          /* QS: */
        f->r+=rdelta[f->movesnum%5];                             /* QS:l9f1k */
        f->c+=cdelta[f->movesnum%5];                             /* QS:m0i7j */
        f->r=f->r % f->h;                                        /* QS:m1l3j */
        f->c=f->c % f->w;                                        /* QS:m2o9i */
    }                                                                 /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
Stack* allocStack(void) {                                             /* QS: */
    Stack* s=malloc(sizeof(Stack));                                   /* QS: */
    s->values=malloc(sizeof(unsigned int)*20);                        /* QS: */
    s->size=0;                                                        /* QS: */
    s->cap=10;                                                        /* QS: */
    return s;                                                         /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void freeStack(Stack* s)                                              /* QS: */
{                                                                     /* QS: */
    if (s!=0)                                                         /* QS: */
    {                                                                 /* QS: */
        free(s->values);                                              /* QS: */
        free(s);                                                      /* QS: */
    }                                                                 /* QS: */
}                                                                     /* QS: */
                                                                      /* QS: */
void push(Stack* s, unsigned int r,unsigned int c) {                  /* QS: */
    if (s->size==(s->cap-1)) {          // need to resize             /* QS: */
        s->cap=s->cap*1.5;      // this is error prone if malloc fails/* QS: */
        unsigned int* temp=malloc(2*sizeof(unsigned int)*s->cap);     /* QS: */
          // but I'm not expecting you to deal with that case         /* QS: */
          // copy over the old contents                               /* QS: */
        for (unsigned int i=0;i<s->size;++i) {                        /* QS: */
            temp[2*i]=s->values[2*i];                            /* QS:m3r5i */
            temp[2*i+1]=s->values[2*i+1];                        /* QS:m4u1i */
        }                                                             /* QS: */
        free(s->values);                                              /* QS: */
        s->values=temp;                                               /* QS: */
    }                                                                 /* QS: */
    s->values[2*s->size]=r;                                      /* QS:m5x7h */
    s->values[2*s->size+1]=c;                                    /* QS:m6a4h */
    s->size++;                                                   /* QS:m7d0h */
}                                                                     /* QS: */
                                                                      /* QS: */
bool pop(Stack* s, unsigned int* r, unsigned int* c) {                /* QS: */
    if (s->size==0) {                                                 /* QS: */
        return false;                                                 /* QS: */
    }                                                                 /* QS: */
    unsigned int pos=s->size-1;                                       /* QS: */
    *r=s->values[2*pos];                                         /* QS:m8g6g */
    *c=s->values[2*pos+1];                                       /* QS:m9j2g */
    s->size--;                                                   /* QS:n0m8f */
    return true;                                                      /* QS: */
}                                                                     /* QS: */
