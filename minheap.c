#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "minheap.h"
#include "avl.h"

/*int main(int argc, char* argv[]){
    treeNode* test1 = (treeNode*) malloc(sizeof(treeNode));
    treeNode* test2 = (treeNode*) malloc(sizeof(treeNode));
    test1->freq = 10;
    test2->freq = 15;
    insertHeap(test1);
    insertHeap(test2);
    int i;
    for(i = 0; i < HeapSize; i++){
        printf("%d\n", minHeap[i]->freq);
    }
}*/
void swap(treeNode** a, treeNode** b){
    treeNode* temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(treeNode* minHeap[], int node){
    if (HeapSize == 1){
        printf("single element heap");
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

treeNode* pop(treeNode* minHeap[]){
    swap(&minHeap[0], &minHeap[HeapSize -1]);
    HeapSize-=1;
    int i;
    for(i = HeapSize/2 -1; i >=0; i--){
        heapify(minHeap, i);
    }
    return minHeap[HeapSize];
}

treeNode* merge(treeNode* less, treeNode* great){
    treeNode* newNode = (treeNode*) malloc(sizeof(treeNode));
    newNode->freq = less->freq + great->freq;
    newNode->token = "\0";
    newNode->left = less;
    newNode->right = great;
    return newNode;
}

