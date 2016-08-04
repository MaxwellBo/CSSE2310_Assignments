typedef struct Node {
    char contents;
    bool seen; 
    struct Node *up;
    struct Node *down;
    struct Node *left;
    struct Node *right;
} Node;
