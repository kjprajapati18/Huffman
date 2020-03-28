#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "avl.h"
#define COUNT 10
int main(int argc, char* argv[]){
    char* test1 = "test1";
    char* test2 = "test2";
    char* test3 = "add";
    char* test4 = "badd";
    char* test5 = "aed";
    char* test6 = "aee";
    Node* head = NULL;
    head = insert(head, test1);
    head = insert(head, test2);
    head = insert(head, test3);
    head = insert(head, test4);
    head = insert(head, test5);
    head = insert(head, test6);
    print2D(head);
}
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
Node* makeNode(char* word){
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->string = (char*) malloc(sizeof(char) * (strlen(word)+1));
    strcpy(newNode->string, word);
    newNode->val = 1;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = 0;
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

int balanceFactor(Node* head){
    if(head == NULL) return 0;
    return height(head->left) - height(head->right);
}

Node* insert(Node* node, char* word){
    if(word[0] == '\0') return node;
    if(node == NULL) return makeNode(word);

    if(strcmp(word, node->string) < 0)
        node->left = insert(node->left, word);
    else if(strcmp(word, node->string) > 0)
        node->right = insert(node->right, word);
    else{
        (node->val)++;
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balanceFactor(node);
    printf("\n\nBalance Factor: %d\n\n", balance);
    //left right or left left
    if(balance > 1){
        //left left
        if(balanceFactor(node->left) >0)
            return RR(node);
        //left right
        else if(balanceFactor(node->left) < 0)
        {
            node->left = LR(node->left);
            return RR(node);
        }
    }
    //right right or right left
    else if(balance < -1){
        //right right
        if (balanceFactor(node->right) < 0){
            return LR(node);
        }
        //right left
        else if(balanceFactor(node->right) >0){
            node->left = RR(node->left);
            return LR(node);
        }
    }
    printf("Inserted %s\n", word);
    return node;
}

void freeAvl(Node* head){
    Node* l = head->left;
    Node* r = head->right;
    free(head);
    freeAvl(l);
    freeAvl(r);
}

void printPreOrder(Node* root){
    if(root != NULL){
        printf("%s\n", root->string);
        printPreOrder(root->left);
        printPreOrder(root->right);
    }
}

void print2DUtil(Node* root, int space){
    if (root == NULL){
        return;
    }
    space += COUNT;
    print2DUtil(root->right, space);
    printf("\n");
    int i;
    for(i = COUNT; i < space; i++){
        printf(" ");
    }
    printf("%s\n", root->string);
    print2DUtil(root->left, space);
}

void print2D(Node* root){
    print2DUtil(root, 0);
}