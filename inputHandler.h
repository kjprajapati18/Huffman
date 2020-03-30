#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include "avl.h"
#define _ESCAPECHAR '\\'

//input flags global
int recursive, build, compress, decomp;


//checks the first 2 flags to see what we're doing and if it's recursive or not
int flagCheck(char**);

//prints all files in a directory and all subdirectories. just a checking method to see if recursivve directory traversal works
void printFiles(DIR*, char*);

//makes printing error messages cleaner
void errorPrint(const char*, int);

//reading function to get all input from file
int fillAVL(Node**, int, char**);

//handles incoming escape characters in file
int incEscapeChar(char**, int*);

#endif 