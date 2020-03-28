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

//minheap array
treeNode* minHeap[100];
//size of heap
int HeapSize = 0;
//heapify method
void heapify(int);

//insert method
void insertHeap(treeNode*);

//delete root method
treeNode* pop();

//swap function
void swap(treeNode**, treeNode**);

//tree merge
treeNode* merge(treeNode*, treeNode*);
#endif