#ifndef AVL_H
#define AVL_H

//tree nodes hold a string and an int- word and freq
typedef struct  node{
    char* string;
    char* codeString;
    int val;
    struct node* left;
    struct node* right;
    int height;
}Node;

//find max of 2 ints
int max(int, int);
//get Height of node
int height(Node*);
//Make node given token&bitString
Node* makeNode(char*, char*);
//Right Rotate for AVL
Node* RR(Node*);
//Left Rotate for AVL
Node* LR(Node*);
//Get balance factor of Node
int balanceFactor(Node*);
//insert a node to AVL and rotate if needed
Node* insert(Node*, char*, char*);
//Free AVL tree
void freeAvl(Node*);
//Print AVL tree, used for testing only
void print2DTree(Node* root, int space);
//Find AVL Node given the token
int findAVLNode(Node**, Node*, char*);
//Rebuild the huffman tree given the head to huffman and a bitString
Node* rebuildHuffman(Node* head, char* token, char* bitString);


int tokens; //Number of tokens (used in Build)

#endif