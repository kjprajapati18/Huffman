#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
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
    if(bcdFlag & (_COMPRESS|_DECOMPRESS) && strcmp("HuffmanCodebook", argv[3+recursive]+strlen(argv[3+recursive])-15)) errorPrint("Fatal Error: The codebook should be called 'HuffmanCodebook.'", 1);

    //Check the file extension to make sure that only decompress gets .hcz
    if(!recursive){
        int indexOfExtension = strlen(argv[2])-4;   //If the extension is more than 3 chars, then its not .hcz anyways
        switch (bcdFlag){
            case _BUILD:
            case _COMPRESS:
                if(strcmp(argv[2]+indexOfExtension, ".hcz") == 0) errorPrint("Fatal Error: You cannot compress a .hcz file or build a codebook with it.", 1);
                break;
            case _DECOMPRESS:
                if(strcmp(argv[2]+indexOfExtension, ".hcz") != 0) errorPrint("Fatal Error: You can only decompress a .hcz file.", 1);
                break;
            default:
                break;
        }
    }
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

                buffer[i] = '\0';

                if(carryOverSize != 0){ //realloc, add tree, check/change escape char
                    carryOverSize += i-startIndex;
                    char* temp = (char*) malloc(sizeof(char)*(carryOverSize+1));
                    memcpy(temp, carryOver, carryOverSize-(i-startIndex)+1);
                    strcat(temp, buffer+startIndex);
                    free(carryOver);
                    carryOver = temp;

                    readHandler(&list, carryOver, carryOverSize, escapeChar, &escapeCharSize, outputFd, mode);
                    carryOverSize = 0;
                } else {
                    readHandler(&list, buffer+startIndex, i-startIndex, escapeChar, &escapeCharSize, outputFd, mode);
                }

	            startIndex = i+1;
                readHandler(&list, delimiter, 1, escapeChar, &escapeCharSize, outputFd, mode); //Add the space that caused us to enter this point
            }
        }

        //Adjusting carryover
        if(startIndex != bytesRead){
            if(carryOverSize == 0){
                free(carryOver);
                carryOverSize = bytesRead-startIndex;
                carryOver = (char *) malloc(sizeof(char) * (carryOverSize+1));
                *carryOver = '\0';
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
        readHandler(&list, carryOver, carryOverSize, escapeChar, &escapeCharSize, outputFd, mode);
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


int readHandler(Node** head, char* token, int tokenSize, char** escapeChar, int* escapeCharSize, int outputFd, int mode){
    Node* selectedNode;
    int found;
    switch(mode){
        case _BUILD:
                
            if(tokenSize == (*escapeCharSize)+1 && strncmp(token, *escapeChar, *escapeCharSize)==0){
                incEscapeChar(escapeChar, escapeCharSize);
            }
            *head = insert(*head, token, "\0");
            break;
        case _COMPRESS:
            if(*token == '\0') return 0;
            found = findAVLNode(&selectedNode, *head, token);
            if(found != 0){
                printf("Fatal Error: No huffman code exists for a token: %s\n", token);
                exit(1);
            }
            writeString(outputFd, selectedNode->codeString);
            break;
        case _DECOMPRESS:
        default:
            errorPrint("Fatal Error: Bad flag...", 1);
            break;
        }
    return 0;//change to vodi prolly
}

Node* codebookAvl(int bookfd, Node* (*treeInsert)(Node*, char*, char*)){
    int bytesRead = 0;
    char buffer[201];
    char carry[201];
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
            free(escapeChar);
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
    int len = strlen(escapeChar);
    lseek(bookfd, len+1, 0);
    Node* head = NULL;
    char* word = (char*) malloc(sizeof(char));
    *word= '\0';
    char* code = (char*) malloc(sizeof(char));
    *code = '\0';
    do{
        bytesRead = read(bookfd, buffer, 200);
        if (bytesRead == -1) return NULL;
        int index = 0;
        int startIndex = 0;
        buffer[bytesRead] = '\0';
        
        while (index <= bytesRead){
            
            if(buffer[index] == '\t'){
                codeTemp = (char*) malloc(codeCarryOverSize + (index - startIndex)+1);
                *codeTemp = '\0';
                buffer[index] = '\0';
                strcpy(codeTemp, code);
                strcat(codeTemp, buffer+startIndex);
                free(code);
                code = codeTemp;
                codeBool = 0; codeCarryOverSize = 0;
                startIndex = index +1;
            }
            else if(buffer[index] == '\n'){
                wordTemp = (char*) malloc(index -startIndex + wordCarryOverSize+1);
                buffer[index] = '\0';
                *wordTemp = '\0';
                strcpy(wordTemp, word);
                strcat(wordTemp, buffer+startIndex);
                free(word);
                word = wordTemp;
                wordCarryOverSize = 0; codeBool = 1;

                if(strncmp(word, escapeChar, len) == 0){
                    char controlCode = word[len];
                    free(word);
                    word = malloc(sizeof(char)*(2));
                    if(controlCode == 'n'){
                        *word = '\n';
                    }
                    else if(controlCode == 't'){
                        *word = '\t';
                    }
                    else if(controlCode == 'r'){
                        *word = '\r';
                    }
                    word[1] = '\0';
                }
                
                head = treeInsert(head, word, code);
                *word = '\0';
                *code = '\0';
                startIndex = index +1;
            }
            else if(buffer[index] == '\0'){
                if(codeBool){
                    codeTemp = (char*) malloc(index - startIndex + codeCarryOverSize+1);
                    *codeTemp = '\0';
                    strcpy(codeTemp, code);
                    strcat(codeTemp, buffer+startIndex);
                    free(code);
                    code = codeTemp;
                    codeCarryOverSize += index-startIndex;
                }
                else{
                    wordTemp = (char*) malloc(index - startIndex + wordCarryOverSize+1);
                    *wordTemp = '\0';
                    strcpy(wordTemp, word);
                    strcat(wordTemp, buffer+startIndex);
                    free(word);
                    word = wordTemp;
                    wordCarryOverSize += index - startIndex;
                }
            }
            
            index++;
        }
    }while(bytesRead > 0);
    return head;
}

int decompressFile(Node* head, int input, int output){
    
    if(head == NULL) return -1;

    int bytesRead = 1;
    char buffer[201];

    //If words aren't complete by the time read returns, we need to carry the word over.
    Node* ptr = head;
    int carryOverSize = 0;
    int escapeCharSize = 1;
    int i;
    


    do{
        bytesRead = read(input, buffer, 200);
        if(bytesRead == -1) return -1;
        else if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';
        for(i = 0; i<bytesRead; i++){
            switch(buffer[i]){
                case '0':
                    ptr = ptr->left;
                    break;
                case '1':
                    ptr = ptr->right;
                    break;
                case '\0':
                    continue;
                    break;
                default:
                    errorPrint("Fatal Error: .hcz file contains invalid characters", 1);
                    break;
            }

            if(ptr->left == NULL && ptr->right == NULL){
                writeString(output, ptr->string);
                ptr = head;
            }
        }
    } while (bytesRead >0);

    if(ptr != head){ //there was leftover or extra bits, caused by changed encryption, changed codebook, or incorrect codebook
        printf("Error: Codebook did not line up with encryption. Left over bits were thrown out\n");
    }

    return 0;
}