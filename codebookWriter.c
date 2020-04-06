#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "codebookWriter.h"

//Traverses the Huffman Tree and recursively write the the output file. Replaces all control codes with escape sequence, then letter
int writeCodebook(treeNode* head, int fd, char* escapeChar, char* bitString){

    if(head == NULL) return -1;     //There is no HuffmanTree

    int bitLength = strlen(bitString);
    if(head->left == NULL && head->right == NULL){                                      //Leaf Node, add to book
        int booleanIsSpace = isspace(head->token[0]) && strcmp(head->token, " ");       //Is a control character space
        char* temp;
        int size = strlen(escapeChar);
        
        //Malloc space depending on size of string that will be written
        if(booleanIsSpace){
            temp = (char*) malloc((bitLength+size+4)*sizeof(char));                     //bitLength + escapeCharSize + 4 (null term, tab, newline, & controlcode letter)
        } else {
            temp = (char*) malloc((bitLength+strlen(head->token)+3)*sizeof(char));     //bitLength(for code) + strlen(token) + 3 (for null-term, tab, and newline)
        }    

        memcpy(temp, bitString, bitLength);                                             //Format for output is <bitString><tab><token><newline>
        temp[bitLength] = '\t';
        temp[bitLength+1] = '\0';
        
        //Determine whether to replace the token with an escape sequence or not
        char* inputtedToken = booleanIsSpace? escapeCharHandler(escapeChar, head->token) : head->token;

        strcat(temp, inputtedToken);                                                    //Finish the rest of the string
        strcat(temp, "\n");

        writeString(fd, temp);                                                          //Output String
        
        //Free and return
        free(temp);
        if(booleanIsSpace) free(inputtedToken);
        return 0;
    }

    //If this point is reached, then there are children. The only time the HuffmanTree is not strictly binary is if there was only 1 token
    //We want to keep track of where we have went, we do this by appending either a 1 or 0 to bitString based on what branch we took
    char* newString = (char *) malloc(sizeof(char) * (bitLength+2));
    memcpy(newString, bitString, bitLength+1);
    newString[bitLength+1] = '\0';
    
    newString[bitLength] = '0';
    int stringLength = strlen(newString);
    if(head->left != NULL)writeCodebook(head->left, fd, escapeChar, newString);

    newString[bitLength] = '1';
    if(head->right != NULL)writeCodebook(head->right, fd, escapeChar, newString);

    //We've completed this node, free and return
    free(newString);
    return 0;
}

//Using a do-while loop to write a String to the output file, which takes care of the non-blocking write issues
//Also gets rid of the null-terminator before writing to avoid garbage characters
//Returns 0 on success and negative number on fail
int writeString(int fd, char* string){

    int size = strlen(string), written = 0, status = 0;

    do{
        status = write(fd, string+written, size-written);
        written += status;
        if(status <0) return -1;
    }while(written != size);

    return 0;
}

//The function is only called when we know that token is a control code. Replcae the \ with the escape sequence (must create a new string)
char* escapeCharHandler(char* escapeChar, char* token){
    int size = strlen(escapeChar);
    char* controlString = (char*) malloc(sizeof(char) * (size+2));
    memcpy(controlString, escapeChar, size+1);
    controlString[size+1] = '\0';

    switch (token[0]){
        case '\t':
            controlString[size] = 't';
            break;
        case '\n':
            controlString[size] = 'n';
            break;
        case '\r':
            controlString[size] = 'r';
            break;
        default:
            break;
    }

    return controlString;

}
