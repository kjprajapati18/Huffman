#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include "inputHandler.h"
#include "codebookWriter.h"

int flagCheck(int argc, char* argv[]){
    int pos;
    int bcdFlag = 0;
    for(pos = 1; pos < 3; pos++){       //there can only be at most 2 flags and they must be the first 2 args
        if(*argv[pos] == '-'){          //Check if its a flag
            switch (*(argv[pos]+1)){    //Check which flag and set appropriate variable
                case 'b':
                    printf("Using Build Huffman Codebook Flag\n");
                    bcdFlag |= _BUILD;
                    break;
                case 'c':
                    printf("Using Compression Flag\n");
                    bcdFlag |= _COMPRESS;
                    break;
                case 'd':
                    printf("Using the Decompression Flag\n");
                    bcdFlag |= _DECOMPRESS;
                    break;
                case 'R':
                    printf("Using the Recursion Flag\n");
                    recursive = 1;
                    break;
            }
        } else break;
    }

    
    //If the user did not pick build, compress, or decompress, or they picked too many. The user may have also not put flags as the first argument
    if(bcdFlag != _BUILD && bcdFlag != _COMPRESS && bcdFlag != _DECOMPRESS) errorPrint("Fatal Error: Invalid flag usage. Make sure to pick exactly 1 flag from the following, and that it comes before any arguments: (-b)uild, (-c)ompress, (-d)ecompress", 1);
    
    //build should have 3 arguments and compress/decompress should have 4 flags (+1 if recursive flag included)
    if((bcdFlag == _BUILD && argc != 3+recursive) || (bcdFlag & (_COMPRESS|_DECOMPRESS) && argc != 4+recursive)) errorPrint("Fatal Error: Incorrect number of arguments for given flags", 1);

    //Make sure that a proper codebook is given if we are compressing or decompressing
    if(bcdFlag & (_COMPRESS|_DECOMPRESS) && strcmp("HuffmanCodebook", argv[3+recursive])) errorPrint("Fatal Error: The codebook should be called 'HuffmanCodebook.'", 1);

    return bcdFlag;
}

void printFiles(DIR* directory, char* basePath){
    char path[NAME_MAX + strlen(basePath)];
    struct dirent* dir;
    while((dir = readdir(directory)) != NULL){
        if(dir->d_type == 8){
            printf("%s\n", dir->d_name);
        }
    }
    printf("\n");
    rewinddir(directory);
    while((dir = readdir(directory)) != NULL){
        if(dir->d_type == 4){
            if((dir->d_name[0]) == '.') continue;
            strcpy(path, basePath);
            strcat(path, dir->d_name);
            strcat(path, "/");
            DIR* direct = opendir(path);
            printf("Files in %s: \n", dir->d_name);
            printFiles(direct, path);
        }
        
    }
}

void errorPrint(const char* message, int exitCode){
     printf("%s\n", message);
    if(exitCode != 0) exit(exitCode);
}

int getInput(Node** head, int inputFd, char** escapeChar, int outputFd, int mode){

    Node* list = *head;
    int bytesRead = 1;
    int size = 0;
    char buffer[201];
    int i = 0;
    int broke = 0;
    char delimiter[2];      //Takes care of adding whitespaces
    delimiter[1] = '\0';

    //If words aren't complete by the time read returns, we need to carry the word over.
    int carryOverSize = 0;
    int escapeCharSize = 1;
    char* carryOver = (char*) malloc(sizeof(char)); //Initialize with size 1 byte because the loop starts with freeing the old value.
    *carryOver = '\0';
  
    do{
        bytesRead = read(inputFd, buffer, 200);
        if(bytesRead == -1) return -1;
        else if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';

        int startIndex = 0;
        for(i = 0; i<bytesRead; i++){
            if (buffer[i] == '\0') break;       //I don't think this line is needed but im too scared to remove
	        if (isspace(buffer[i])){
                delimiter[0] = buffer[i];
                readHandler(&list, delimiter, 1, escapeChar, escapeCharSize, outputFd, mode);                            //list = insert(list, delimiter);
                buffer[i] = '\0';

                if(carryOverSize != 0){ //realloc, add tree, check/change escape char
                    carryOverSize += i-startIndex;
                    char* temp = (char*) malloc(sizeof(char)*(carryOverSize+1));
                    memcpy(temp, carryOver, carryOverSize-(i-startIndex)+1);
                    strcat(temp, buffer+startIndex);
                    free(carryOver);
                    carryOver = temp;

                    readHandler(&list, carryOver, carryOverSize, escapeChar, escapeCharSize, outputFd, mode);
                    /*
                    if(carryOverSize == escapeCharSize+1 && strncmp(carryOver,*escapeChar,escapeCharSize)==0){
                        incEscapeChar(escapeChar, &escapeCharSize);
                    }
                    list = insert(list, carryOver);*/
                    carryOverSize = 0;
                } else {
                    readHandler(&list, buffer+startIndex, i-startIndex, escapeChar, escapeCharSize, outputFd, mode);
                    /*if(i-startIndex == escapeCharSize+1 && strncmp(buffer+startIndex,*escapeChar,escapeCharSize)==0){
                        incEscapeChar(escapeChar, &escapeCharSize);
                    }
                    list = insert(list, buffer+startIndex);*/
                }

	            startIndex = i+1;
            }
        }

        //Adjusting carryover
        if(startIndex != bytesRead){
            if(carryOverSize == 0){
                free(carryOver);
                carryOverSize = bytesRead-startIndex;
                carryOver = (char *) malloc(sizeof(char) * (carryOverSize+1));
                strcpy(carryOver, buffer+startIndex);
            } else {
                carryOverSize += bytesRead-startIndex;
                char* temp = (char*) malloc(sizeof(char)*(carryOverSize+1));
                memcpy(temp, carryOver, strlen(carryOver)+1);
                strcat(temp, buffer+startIndex);
                free(carryOver);
                carryOver = temp;
            }
        }
        
    }while(bytesRead>0);
    
    if(carryOverSize !=0){
        readHandler(&list, carryOver, carryOverSize, escapeChar, escapeCharSize, outputFd, mode);
        /*if(carryOverSize == escapeCharSize+1 && strncmp(carryOver,*escapeChar,escapeCharSize)==0){
            incEscapeChar(escapeChar, &escapeCharSize);
        }
        list = insert(list, carryOver);*/
    }

    free(carryOver);
    *head =list;
    return 0;
}

int incEscapeChar(char** escapeChar, int* escapeCharSize){

    if(escapeChar == NULL) return -1;
    *escapeCharSize *= 2;
    char* temp = (char*) malloc(sizeof(char)* (*(escapeCharSize)+1));
    if(temp == NULL) return 1;
    
    int i;
    for(i = 0; i<*escapeCharSize; i++) temp[i] = _ESCAPECHAR;
    temp[*escapeCharSize] = '\0';
    
    free(*escapeChar);
    *escapeChar = temp;
    return 0;
}

int readHandler(Node** head, char* token, int tokenSize, char** escapeChar, int escapeCharSize, int outputFd, int mode){

        switch(mode){
            case _BUILD:
                if(tokenSize == escapeCharSize+1 && strncmp(token,*escapeChar,escapeCharSize)==0){
                    incEscapeChar(escapeChar, &escapeCharSize);
                }
                *head = insert(*head, token, "\0");
                break;
            case _COMPRESS:
<<<<<<< HEAD
                Node* selectedNode= NULL;
                int found = findAVLNode(&selectedNode, *head, token);
                if(found != 0) errorPrint("FATAL ERROR: No code exists for this word", 1);
                char toWrite[20];
                sprintf(toWrite, "%d", selectedNode->val);
                writeString(outputFd, toWrite);
=======
                found = findAVLNode(&selectedNode, *head, token);
                if(found != 0){
                    printf("Fatal Error: No huffman code exists for a token: %s\n", token);
                    exit(1);
                }
                writeString(outputFd, selectedNode->codeString);
>>>>>>> ba0b09fc93c37627e38f75bed4216a2d66de2f45
                break;
            case _DECOMPRESS:

                break;
            default:
                break;
        }

}

Node* codebookAvl(int bookfd, Node* (*treeInsert)(Node*, char*, char*)){
    int bytesRead = 0;
    char[201] buffer;
    char[201] carry;
    char* escapeChar = (char*) malloc(sizeof(char));
    *escapeChar = '\0';
    char* escapeTemp;
    int carryOverbool = 0;
    int carryOverSize = 0;
    do{
        bytesRead = read(bookfd, buffer, 200);
        if(bytesRead == -1) return NULL;
        int index = 0;
        buffer[bytesRead] = '\0';
        while(buffer[index] != '\n' && buffer[index] != '\0'){
            carry[index] = buffer[index];
            index++;
        }
        if(buffer[index] == '\n'){
            escapeTemp = (char*) malloc(carryOverSize+ index);
            carry[index] = '\0';
            strcpy(escapeTemp, escapeChar);
            strcat(escapeTemp, carry);
            free(escapeChar);
            escapeChar = escapeTemp;
            lseek(bookfd, SEEK_CUR, index+1-bytesRead +1);
            break;
        }

        if(carryOverbool){
            escapeTemp = (char*) malloc(index + carryOverSize +1);
            carry[index] = '\0';
            strcpy(escapeTemp, escapeChar);
            strcat(escapeTemp, carry);
            free(escapeChar);
            escapeChar = escapeTemp;
            carryOverSize+= bytesRead;
        }
        else{
            carryOverbool = 1;
            escapeChar = (char*) malloc(bytesRead+1);
            carry[index] = '\0';
            strcpy(escapeChar, carry);
            carryOverSize += bytesRead;
        }
        
        
    }while(bytesRead >0);

    bytesRead = 0;
    char* wordTemp;
    
    char* codeTemp;
    int codeBool = 1;
    int wordCarryOverSize = 0;
    int codeCarryOverSize = 0;
    int startIndex = 0;
    Node* head = NULL;
    do{
        bytesRead = read(bookfd, buffer, 200);
        if (bytesRead == -1) return NULL;
        int index = 0;
        buffer[bytesRead] = '\0';
        char* word = (char*) malloc(sizeof(char));
        *word= '\0';
        char* code = (char*) malloc(sizeof(char));
        *code = '\0';
        while (index <= bytesRead){
            if(buffer[index] == '\t'){
                codeTemp = (char*) malloc(codeCarryOverSize + (index - startIndex));
                buffer[index] = '\0';
                strcpy(codeTemp, code);
                strcat(codeTemp, buffer[startIndex]);
                free(code);
                code = codeTemp;
                codeBool = 0; codeCarryOverSize = 0;
                startIndex = index +1;
            }
            else if(buffer[index] == '\n'){
                wordTemp = (char*) malloc(index -startIndex + wordCarryOverSize);
                buffer[index] = '\0';
                strcpy(wordTemp, word);
                strcat(wordTemp, buffer[startIndex]);
                free(word);
                word = wordTemp;
                wordCarryOverSize = 0; codeBool = 1;
                int len = strlen(escapeChar);
                if(strncmp(word, escapeChar, len) == 0){
                    if(word[len] == 'n'){
                        free(word);
                        *word = '\n';
                    }
                    else if(word[len] == 't'){
                        free(word);
                        *word = '\t';
                    }
                    else if(word[len] == 'r'){
                        free(word);
                        *word = '\r';
                    }
                }
                head = treeInsert(head, word, code);
            }
            else if(buffer[index] == '\0'){
                if(codeBool){
                    codeTemp = (char*) malloc(index - startIndex + codeCarryOverSize);
                    strcpy(codeTemp, code);
                    strcat(codeTemp, buffer[startIndex]);
                    free(code);
                    code = codeTemp;
                    codeCarryOverSize += index-startIndex;
                }
                else{
                    wordTemp = (char*) malloc(index - startIndex + wordCarryOverSize);
                    strcpy(wordTemp, word);
                    strcat(wordTemp, buffer[startIndex]);
                    free(word);
                    word = wordTemp;
                    wordCarryOverSize += index - startIndex;
                }
            }
            index++;
        }
            /*
            if(wordCarryOverbool){
                wordTemp = (char*) malloc(sizeof(char)*index + wordCarryOverSize+1);
                carry[index] = '\0';
                strcpy(wordTemp, word);
                strcat(wordTemp, carry);
                free(word);
                word = wordTemp;
                wordCarryOverSize += bytesRead; 
            }
            else if(wordBool){
                wordCarryOverbool = 1;
                free(word);
                word = (char*) malloc(sizeof(char)*bytesRead+1);
                carry[index] = '\0';
                strcpy(word, carry);
                wordCarryOverSize+= bytesRead;
            }
            if(codeCarryOverbool){
                codeTemp = (char*) malloc(sizeof(char)* index + codeCarryOverSize+1);
                carry[index] = '\0';
                strcpy(codeTemp, code);
                strcat(codeTemp, carry);
                free(code);
                code = codeTemp;
                codeCarryOverSize += bytesRead;
            }
            else if(codeBool){
                codeCarryOverbool = 1;
                free(code);
                code = (char*) malloc(sizeof(char)*bytesRead+1);
                carry[index] = '\0';
                strcpy(code, carry);
                codeCarryOverSize+=bytesRead;
            }*/
    }while(bytesRead > 0);
    return head;
}