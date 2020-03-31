#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include "avl.h"
#define _ESCAPECHAR '\\'
#define _BUILD 0x1
#define _COMPRESS 0x2
#define _DECOMPRESS 0x4

//recursive global flag
int recursive;


//checks the first 2 flags to see what we're doing and if it's recursive or not
int flagCheck(int, char**);

//prints all files in a directory and all subdirectories. just a checking method to see if recursivve directory traversal works
void printFiles(DIR*, char*);

//makes printing error messages cleaner
void errorPrint(const char*, int);

//reading function to get all input from file
int getInput(Node**, int, char**, int, int);

//handles incoming escape characters in file
int incEscapeChar(char**, int*);

int readHandler(Node**, char*, int, char**, int, int, int);


#endif 