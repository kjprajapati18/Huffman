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
Node* makeNode(char*);
Node* RR(Node*);
Node* LR(Node*);
int balanceFactor(Node*);
Node* insert(Node*, char*);
void freeAvl(Node*);
void printPreOrder(Node*);
void print2DTree(Node* root, int space);


#endif