#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "minheap.h"

//Swap 2 tree Nodes
void swap(treeNode** a, treeNode** b){
    treeNode* temp = *a;
    *a = *b;
    *b = temp;
}

//Turn an unordered array into a heap (algo from data structures)
void heapify(treeNode* minHeap[], int node){
    
    if (HeapSize == 1){
        //printf("single element heap");
    }
    else{
        int min = minHeap[node]->freq;
        int minIndex = node;
        if(2*node+1 < HeapSize)
        {
            int l = minHeap[2*node +1]->freq;
            if(l < min) {
                min = l;
                minIndex = 2*node+1;
            }
        }
        if(2*node+2 < HeapSize){
            int r = minHeap[2*node +2]->freq;
            if(r<min) {
                min = r;
                minIndex = 2*node+2;
            }
        }
        if(minIndex != node){
            swap(&minHeap[node], &minHeap[minIndex]);
            heapify(minHeap, minIndex);
        }
    }

}

//Puts a treeNode into the array and maintain the heap structure
void insertHeap(treeNode* minHeap[], treeNode* newNode){
    if(HeapSize == 0){
        minHeap[0] = newNode;
        HeapSize+=1;
    }
    else{
        minHeap[HeapSize] = newNode;
        HeapSize+=1;
        int i = HeapSize/2 -1;
        for (i; i >= 0; i--){
            heapify(minHeap, i);
        }
    }
}

//Remove an element from the array and maintain heap structure
treeNode* pop(treeNode* minHeap[]){
    swap(&minHeap[0], &minHeap[HeapSize -1]);
    HeapSize-=1;
    int i;
    heapify(minHeap, 0);
    return minHeap[HeapSize];
}

//Create a new Node that points to both less and great. This is used when creating a huffman tree, which is when we need to create intermediate trees with summed frequencies
treeNode* merge(treeNode* less, treeNode* great){
    treeNode* newNode = (treeNode*) malloc(sizeof(treeNode));
    newNode->freq = less->freq + great->freq;
    newNode->token = (char*) malloc(sizeof(char));
    *(newNode->token) = '\0';
    newNode->left = less;
    newNode->right = great;
    return newNode;
}

//Free Everythign in Huffman tree (token and nodes)
void freeHuff(treeNode* head){
    treeNode* l = head->left;
    treeNode* r = head->right;
    free(head->token);
    free(head);
    if(l != NULL) freeHuff(l);
    if(r != NULL) freeHuff(r);
}


//This function is used for testing only. It prints out the huffman tree sideways
void print2DTreeNode(treeNode *root, int space) 
{ 
    // Base case 
    if (root == NULL) 
        return; 
  
    // Increase distance between levels 
    space += 10; 
  
    // Process right child first 
    print2DTreeNode(root->right, space); 
  
    // Print current node after space 
    // count 
    printf("\n");
    int i; 
    for (i = 10; i < space; i++) 
        printf(" ");
    if(strcmp(" ", root->token) == 0) printf("[space]%d\n",root->freq);
    else if(strcmp("\n", root->token) == 0) printf("[\\n]%d\n",root->freq);
    else if(strcmp("\r", root->token) == 0) printf("[\\r]%d\n",root->freq);
    else printf("%s%d\n", root->token, root->freq); 
  
    // Process left child 
    print2DTreeNode(root->left, space); 
}


//This function takes the AVL tree and turns each Node into a TreeNode. It then puts every TreeNode into an array
int fillMinHeapArray(treeNode*minHeap[], Node* root, int count){
    if(root == NULL) return count;
    int size = strlen(root->string) + 1;
    treeNode* newNode = (treeNode*) malloc(sizeof(treeNode));
    newNode->freq = root->val;
    newNode->token = (char*) malloc(size);
    memcpy(newNode->token, root->string, size);
    newNode->left = NULL;
    newNode->right = NULL;
    minHeap[count] = newNode;
    count++;
    count = fillMinHeapArray(minHeap, root->left, count);
    count = fillMinHeapArray(minHeap, root->right, count);
    return count;
}
