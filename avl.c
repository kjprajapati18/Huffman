#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
    newNode->height = 1;
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

    head->height = max(height(head->left), height(head->right));
    newHead->height = max(height(newHead->left), height(newHead->right));

    return newHead;
}

int balance(Node* head)){
    if(head == NULL) return 0;
    return height(head->left) - height(head->right);
}

Node* insert(Node* node, char* word, int num){
    if(node == NULL) return(newNode(num, word));

    if(strcmp(word, ptr->string) < 0)
        node->left = insert(node->left, word, num);
    else if(strcmp(word, ptr->string) > 0)
        node->right = insert(node->right, word, num);
    else{
        (node->num)++;
        return(node);
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balance(node);

    if(balance > 1){
        if(strcmp(word, node->left->string) < 0)
            return RR(node);
        else if(strcmp(word, node->left->string) > 0)
        {
            node->left = LR(node->left);
            return RR(node);
        }
    }
    else if(balance < -1){
        if (strcmp(word, node->right->string) > 0){
            return LR(node);
        }
        else if(strcmp(word, node->right->string) < 0){
            node->left = RR(node-left);
            return LR(node);
        }
    }
    return node;
}

void freeAvl(Node* head){
    Node* l = head->left;
    Node* r = head->right;
    free(head);
    freeAvl(l);
    freeAvl(r);
}