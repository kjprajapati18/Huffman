#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "codebookWriter.h"

/*int writeCodebook(treeNode* head, int fd, char* escapeChar, char* bitString){
    if(head == NULL) return -1;
    int bitLength = strlen(bitString);

    if(head->left == NULL && head->right == NULL){ //Leaf Node, add to book
        char* temp = (char*) malloc((bitLength+strlen(head->token)+3)*sizeof(char));    //bitLength(for code) + 3 (for null-term, tab, and newline) + strlen(for token)

        memcpy(temp, bitString, bitLength);
        temp[bitLength] = '\t';
        temp[bitLength+1] = '\0';

        int booleanIsSpace = isspace(head->token[0]) && strcmp(head->token, " ");      //Is a control character space

        //Just need to finish writeString function && escapeCharHandler function
        int size = strlen(escapeChar);
        char* controlString = (char*) malloc(sizeof(char) * (size+2));
        memcpy(controlString, escapeChar, size+1);
        controlString[size+1] = '\0';

        switch ((head->token)[0]){
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
        
        char* inputtedToken = booleanIsSpace? controlString : head->token;

        strcat(temp, inputtedToken);
        strcat(temp, "\n");

        writeString(fd, temp);
        
        free(temp);
        free(controlString);
        return 0;
    }

    //If this point is reached, then there are children. Note that Huffman trees are strictly binary
    char* newString = (char *) malloc(sizeof(char) * (bitLength+2));
    memcpy(newString, bitString, bitLength+1);
    newString[bitLength+1] = '\0';
    
    newString[bitLength] = '0';
    int stringLength = strlen(newString);
    if(head->left != NULL)writeCodebook(head->left, fd, escapeChar, newString);

    newString[bitLength] = '1';
    if(head->left != NULL)writeCodebook(head->right, fd, escapeChar, newString);

    free(newString);
    return 0;
}*/

int writeString(int fd, char* string){

    int size = strlen(string), written = 0, status = 0;

    do{
        status = write(fd, string+written, size-written);
        written += status;
        if(status <0) return -1;
    }while(written != size);

    return 0;
}

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