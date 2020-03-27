#ifndef AVL_H
#define AVL_H

//tree nodes hold a string and an int- word and freq
typedef struct  node{
    char* string;
    int val;
    struct node* left;
    struct node* right;
    int height;
}Node;

int max(int, int);
int height(Node*);
Node* makeNode(int, char*);
Node* RR(Node*);
Node* LR(Node*);
Node* insert(Node*);


#endif