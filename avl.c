#include <stdlib.h>
#include <stdio.h>
#include "avl.h"

//helper method to find max of 2 numbers. returns 2nd number if equal
int max(int a, int b){
    return (a>b)? a: b;
}
//returns height of tree from specific node
int height(Node* head){
    if (head == NULL){
        return 0;
    }
    else return head->height;
}
//mallocs new node and sets word and frequency values
Node* makeNode(int num, char* word){
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->string = word;
    newNode->val = num;
    newNode->left = NULL;
    newNode->right = NULL;
    height = 1;
    return newNode;
}
//right rotation helper method to help with rebalancing
Node* RR(Node* head){
    Node* newHead = head->left;
    Node* changes = newHead->right;

    head->left = changes;
    newHead->right = head;

    head->height = max(height(head->left), height(head->right));
    newHead->height = max(height(newHead->left), height(newHead->right));

    return newHead;
}

//left rotation helper method
Node* LR(Node* head){
    Node* newHead = head->right;
    Node* changes = newHead->left;

    head->right = changes;
    newHead->left = head;

    head->height = max(height(head-left), height(head->right));
    newHead->height = max(height(newHead->left), height(newHead->right));

    return newHead;
}

int balance(Node* head)){
    if(head == NULL) return 0;
    return height(head->left) - height(head->right);
}

Node* insert(Node* head){
    
}