#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "avl.h"
#include "inputHandler.h"
#define COUNT 10

/*int main(int argc, char* argv[]){
    char* test1 = "test1";
    char* test2 = "test2";
    char* test3 = "add";
    char* test4 = "badd";
    char* test5 = "aed";
    char* test6 = "aee";
    Node* head = NULL;
    head = insert(head, test1);
    print2DTree(head, 0);
    head = insert(head, test2);
    print2DTree(head, 0);
    head = insert(head, test3);
    print2DTree(head, 0);
    head = insert(head, test4);
    print2DTree(head, 0);
    head = insert(head, test5);
    print2DTree(head, 0);
    head = insert(head, test5);
    print2DTree(head, 0);
    printf("\n\n%d\n\n", tokens);
}*/
//helper method to find max of 2 numbers. returns 2nd number if equal
int max(int a, int b){
    return (a>b)? a: b;
}

//returns height of tree from specific node
int height(Node* head){
    return (head==NULL)? 0 : head->height;
}

//mallocs new node and sets word and frequency values
Node* makeNode(char* word, char* code){
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->string = (char*) malloc(sizeof(char) * (strlen(word)+1));
    strcpy(newNode->string, word);
    newNode->codeString = (char*) malloc(sizeof(char)* (strlen(code)+1));
    strcpy(newNode->codeString, code);
    newNode->val = 1;
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

    head->height = max(height(head->left), height(head->right))+1;                        
    newHead->height = max(height(newHead->left), height(newHead->right))+1;

    return newHead;
}

//left rotation helper method
Node* LR(Node* head){
    Node* newHead = head->right;
    Node* changes = newHead->left;

    head->right = changes;
    newHead->left = head;

    head->height = max(height(head->left), height(head->right))+1;
    newHead->height = max(height(newHead->left), height(newHead->right))+1;

    return newHead;
}

int balanceFactor(Node* head){
    return (head == NULL)? 0: height(head->left) - height(head->right);
}

Node* insert(Node* node, char* word, char* code){
    if(word[0] == '\0') return node;
    if(node == NULL){
        tokens++;
        return makeNode(word, code);
    } 

    if(strcmp(word, node->string) < 0)
        node->left = insert(node->left, word, code);
    else if(strcmp(word, node->string) > 0)
        node->right = insert(node->right, word, code);
    else{
        (node->val)++;
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = balanceFactor(node);
    //left right or left left
    if(balance > 1){
        //left left
        if(balanceFactor(node->left) >= 0){
            return RR(node);
            
        }
        //left right
        else{
            node->left = LR(node->left);
            return RR(node);
        }
    }
    //right right or right left
    else if(balance < -1){
        //right right
        if (balanceFactor(node->right) <= 0){
            return LR(node);
        }
        //right left
        else {
            node->right = RR(node->right);
            return LR(node);
        }
    }
    return node;
}

void freeAvl(Node* head){
    Node* l = head->left;
    Node* r = head->right;
    free(head->string);
    free(head->codeString);
    free(head);
    if(l != NULL) freeAvl(l);
    if(r != NULL) freeAvl(r);
}

void print2DTree(Node *root, int space) 
{ 
    // Base case 
    if (root == NULL) 
        return; 
  
    // Increase distance between levels 
    space += 10; 
  
    // Process right child first 
    print2DTree(root->right, space); 
  
    // Print current node after space 
    // count 
    printf("\n");
    int i; 
    for (i = 10; i < space; i++) 
        printf(" ");
    if(strcmp(" ", root->string) == 0) printf("[space]%d\n",root->val);
    else if(strcmp("\n", root->string) == 0) printf("[\\n]%d\n",root->val);
    else if(strcmp("\r", root->string) == 0) printf("[\\r]%d\n",root->val);
    else printf("%s%d\n", root->string, root->val); 
  
    // Process left child 
    print2DTree(root->left, space); 
}


int findAVLNode(Node** selectedNode, Node* head, char* token){
    int status = 0;
    if(token[0] == '\0') return -1;
    if(head == NULL){
        return -2;
    }

    if(strcmp(token, head->string) < 0)
        status = findAVLNode(selectedNode, head->left, token);
    else if(strcmp(token, head->string) > 0)
        status = findAVLNode(selectedNode, head->right, token);
    else{
        *selectedNode = head;
    }
    return status;
}

Node* rebuildHuffman(Node* head, char* token, char* bitString){
    if(head == NULL){
        head = makeNode(token, bitString);
    }

    if(*bitString == '0'){
        head->left = rebuildHuffman(head->left, token, bitString+1);
    }else if(*bitString == '1'){
        head->right = rebuildHuffman(head->right, token, bitString+1);
    }else if (strlen(bitString) != 0){
        printf("Fatal Error: Huffman code for [%s] is [%s], which is not a bit string\n", token, bitString);
        exit(1);
    }

    return head;
}