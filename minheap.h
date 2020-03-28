#ifndef MINHEAP_H
#define MINHEAP_H
#include "avl.h"
//nodes that are stored in minheap
typedef struct treeNode{
    int freq;
    char* token;
    struct treeNode* left;
    struct treeNode* right;
}treeNode;

//size of heap
int HeapSize = 0;
//heapify method
void heapify(treeNode**, int);

//insert method
void insertHeap(treeNode**, treeNode*);

//delete root method
treeNode* pop(treeNode**);

//swap function
void swap(treeNode**, treeNode**);

//tree merge
treeNode* merge(treeNode*, treeNode*);
#endif