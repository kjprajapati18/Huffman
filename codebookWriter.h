#ifndef CODEBOOKWRITER_H
#define CODEBOOKWRITER_H
#include "avl.h"
#include "minheap.h"

//writes the codebook using the final huffman tree produced
int writeCodebook(treeNode*, int, char*, char*);

//simplifies writing a string to a file to account for blocking
int writeString(int, char*);

//handles escape characters in written file
char* escapeCharHandler(char*, char*);\

#endif