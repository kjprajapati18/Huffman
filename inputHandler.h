#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include "avl.h"
#define _ESCAPECHAR '\\'
#define _BUILD 10
#define _COMPRESS 11
#define _DECOMPRESS 12

//input flags global:::::::::::: NOTE: I want to get rid of at least 3 of these
int recursive, build, compress, decomp;


//checks the first 2 flags to see what we're doing and if it's recursive or not
int flagCheck(int, char**);

//prints all files in a directory and all subdirectories. just a checking method to see if recursivve directory traversal works
void printFiles(DIR*, char*);

//makes printing error messages cleaner
void errorPrint(const char*, int);

//reading function to get all input from file
int getInput(Node**, int, char**, int);

//handles incoming escape characters in file
int incEscapeChar(char**, int*);

int readHandler(Node**, char*, int, char**, int, int);

#endif 