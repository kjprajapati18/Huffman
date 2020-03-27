#include <stdlib.h>

struct typdef node{
    char* string;
    int val;
    struct node* left;
    struct node* right;
    int height;
}Node;

int max(int a, int b){
    return (a>b)? a: b;
}

int height(Node* head){
    if (head == NULL){
        return 0;
    }
    else return head->height;
}

Node* makeNode(int num, char* word){
    newNode = 
}