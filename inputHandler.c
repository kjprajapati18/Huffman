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
    if(strlen(argv[1]) > 2){
        errorPrint("First flag must be either -R -b -c or -d", 1);
    }
    for(pos = 1; pos < 3; pos++){  
        if(pos == 2 && strlen(argv[2]) > 2) continue;     //there can only be at most 2 flags and they must be the first 2 args
        if(*argv[pos] == '-'){          //Check if its a flag
            switch (*(argv[pos]+1)){    //Check which flag and set appropriate variable
                case 'b':
                    bcdFlag |= _BUILD;
                    break;
                case 'c':
                    bcdFlag |= _COMPRESS;
                    break;
                case 'd':
                    bcdFlag |= _DECOMPRESS;
                    break;
                case 'R':
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

//This function was only used for testing purposes
/*
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
            if((dir->d_name[0]) == '.'){
                //printf("Directory: %s\n", dir->d_name);
                continue;
            }
            strcpy(path, basePath);
            strcat(path, dir->d_name);
            strcat(path, "/");
            DIR* direct = opendir(path);
            printf("Files in %s: \n", dir->d_name);
            printFiles(direct, path);
        }
        
    }
}*/

//Prints an error message and exits the program
void errorPrint(const char* message, int exitCode){
     printf("%s\n", message);
    if(exitCode !=0) exit(exitCode);
}

//Reads inputFd and either builds AVL tree in _BUILD mode or writes compressed file in _COMPRESS
int getInput(Node** head, int inputFd, char** escapeChar, int outputFd, int mode){

    Node* list = *head;
    int bytesRead = 1;
    int readHandlerCheck = 0;
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
        //Read 200 bytes. If 0, then we've reached EOF. If -1 something is wrong so return
        bytesRead = read(inputFd, buffer, 200);
        if(bytesRead == -1){
            printf("Fatal Error: read could not read file\n");
            return -1;
        }
        else if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';               //Set end to \0

        int startIndex = 0;                     //Keep track of the start of a token

        for(i = 0; i<bytesRead; i++){
            if (buffer[i] == '\0') break;       //If we hit a null terminator, then we are done with the buffer so far
	        if (isspace(buffer[i])){            //spaces are the delimiter
                delimiter[0] = buffer[i];       //Creates a string with only the delimiter (delimiter[1] already set to \0)

                buffer[i] = '\0';               //Change the delimiter to \0 so we can use string functions with buffer+startIndex

                //If there was something to carryOver previously, then realloc before sending it to ReadHandler
                if(carryOverSize != 0){         
                    carryOverSize += i-startIndex;
                    char* temp = (char*) malloc(sizeof(char)*(carryOverSize+1));
                    memcpy(temp, carryOver, carryOverSize-(i-startIndex)+1);
                    strcat(temp, buffer+startIndex);
                    free(carryOver);
                    carryOver = temp;

                    readHandlerCheck = readHandler(&list, carryOver, carryOverSize, escapeChar, &escapeCharSize, outputFd, mode);
                    carryOverSize = 0;
                } else {
                    readHandlerCheck = readHandler(&list, buffer+startIndex, i-startIndex, escapeChar, &escapeCharSize, outputFd, mode);
                }

                //After sending to read handler, we know that the next word would be at index+1
	            startIndex = i+1;
                //Always check the handler
                if(readHandlerCheck != 0){
                    free(carryOver);
                    *head = list;
                    return -1;
                }
                readHandlerCheck = readHandler(&list, delimiter, 1, escapeChar, &escapeCharSize, outputFd, mode); //Finally, add the space that caused us to enter this point
                if(readHandlerCheck != 0){
                    free(carryOver);
                    *head = list;
                    return -1;
                }
            }
        }

        //Adjusting carryover if there was left over characters at the end of buffer
        if(startIndex != bytesRead){
            //realloc or simply free based on whether there was already a carryOver(if a token was more than 200 bytes, we would enter the else)
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
    
    //if we finished reading a file, but we didn't handle the last token because it didn't end in a space, then we should handle it now
    if(carryOverSize !=0){
        readHandlerCheck = readHandler(&list, carryOver, carryOverSize, escapeChar, &escapeCharSize, outputFd, mode);
    }

    //Free, and set returns
    free(carryOver);
    *head =list;
    if(readHandlerCheck != 0) return -1;
    return 0;
}

//This function is called if we detect that a token is the same as our escape sequence. We will now change the escape sequence
//We change it by doubling it in length. Returns 0 if successulf and 1 on malloc error
int incEscapeChar(char** escapeChar, int* escapeCharSize){

    if(escapeChar == NULL) return -1;
    *escapeCharSize *= 2;
    char* temp = (char*) malloc(sizeof(char)* (*(escapeCharSize)+1));
    if(temp == NULL) return 1;
    
    //Set everything to the escape char after doubling the string size.
    int i;
    for(i = 0; i<*escapeCharSize; i++) temp[i] = _ESCAPECHAR;
    temp[*escapeCharSize] = '\0';
    
    //Free the old escape char and set the new one.
    free(*escapeChar);
    *escapeChar = temp;
    return 0;
}

//Based on the mode (flag), this function will determine what to do with the token that was read, and handling the escapeChar changes
//_BUILD -> function will increment the escapeChar if necessary and then insert the token into AVL
//_COMPRESS -> function will attempt to find the token in the AVL created from the codebook. If it is found, it will write it to the output file
//              Otherwise, it will end the program and say which token was not found
//OTHER ->  This will never occur because of flagCheck
int readHandler(Node** head, char* token, int tokenSize, char** escapeChar, int* escapeCharSize, int outputFd, int mode){
    Node* selectedNode;
    int found;
    switch(mode){
        case _BUILD:   
            if(tokenSize == (*escapeCharSize)+1 && strncmp(token, *escapeChar, *escapeCharSize)==0){
                int mallocCheck = incEscapeChar(escapeChar, escapeCharSize);
                if (mallocCheck != 0){
                    printf("Fatal Error: Not enough memory to perform the operation on this file\n");
                    return -1;
                }
            }
            *head = insert(*head, token, "\0");
            break;

        case _COMPRESS:
            if(*token == '\0') return 0;
            found = findAVLNode(&selectedNode, *head, token);
            if(found != 0){
                printf("Fatal Error: No huffman code exists for a token: %s\n", token);
                return -1;
            }
            writeString(outputFd, selectedNode->codeString);
            break;
        default:
            break;
    }
    return 0;
}

//Reads the AVL codebook. Function point is used to determine whether we want to create a HuffmanTree (rebuildHuffman) or AVL tree(insert)
//The function first reads and finds the escape character in the codebook, then sets the file pointer to the first token, and then begins reading and inserting
Node* codebookAvl(int bookfd, Node* (*treeInsert)(Node*, char*, char*)){
    int bytesRead = 0;
    char buffer[201];
    char carry[201];
    char* escapeChar = (char*) malloc(sizeof(char));
    *escapeChar = '\0';
    char* escapeTemp;
    int carryOverbool = 0;
    int carryOverSize = 0;

    //Get the escape character
    do{
        bytesRead = read(bookfd, buffer, 200);
        if(bytesRead == -1) return NULL;
        int index = 0;
        buffer[bytesRead] = '\0';
        while(buffer[index] != '\n' && buffer[index] != '\0'){
            carry[index] = buffer[index];
            index++;
        }
        
        //escape sequence will end with a newline, so we have found it. Set escapeChar and get out of do-while loop
        if(buffer[index] == '\n'){
            escapeTemp = (char*) malloc(carryOverSize+ index);
            carry[index] = '\0';
            strcpy(escapeTemp, escapeChar);
            strcat(escapeTemp, carry);
            free(escapeChar);
            escapeChar = escapeTemp;
            break;
        }
        
        //If we haven't found it, then we want to carry over and keep reading. carryOverbool checks if this is our first time carrying somethign over
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

    //Set up for next read by declaring vars and lseek to after the escapeChar (incase we read too far)
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
            
            //If we find a \t, then we have finished reading a bit String, store the bitString. (Make sure to include the carryOver if there was any) 
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
            //If we find a \n, then since we have already read the escape sequence, we have read a token. Store the token and try to add token & bitString (make sure to include possible carryOVer)
            else if(buffer[index] == '\n'){
                wordTemp = (char*) malloc(index -startIndex + wordCarryOverSize+1);
                buffer[index] = '\0';
                *wordTemp = '\0';
                strcpy(wordTemp, word);
                strcat(wordTemp, buffer+startIndex);
                free(word);
                word = wordTemp;
                wordCarryOverSize = 0; codeBool = 1;

                //If the token was a representation of a control code, we will detect and replace it here
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
                
                //We have finished cleaning up our input. Add it to the tree using the function pointer. Then reset for the next word
                head = treeInsert(head, word, code);
                *word = '\0';
                *code = '\0';
                startIndex = index +1;
            }
            //If we hit a null-term instead of \t or \n, then we need to carry over whatever we read. codeBool determines whether the remainder is stored in code or word
            //Note that if we stop in the middle of a word, then the bitString is already stored in code
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

    //Free and return
    free(word);
    free(code);
    free(escapeChar);
    return head;
}

//Read a compressed file and use the HuffmanTree to decode it
//Returns negative number on on error and 0 on success
int decompressFile(Node* head, int input, int output){
    
    if(head == NULL){
        return -2;
    }

    int bytesRead = 1;
    char buffer[201];

    Node* ptr = head;           //Create a pointer that will traverse the rebuilt Huffman tree as we read the compressed code.
    int carryOverSize = 0;
    int escapeCharSize = 1;
    int i;
    

    //Read the buffer.
    do{
        bytesRead = read(input, buffer, 200);
        if(bytesRead == -1){
            printf("Fatal Error: read could not read file");
            return -3;
        }
        else if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';
        for(i = 0; i<bytesRead; i++){
            //If we read a 0, move the pointer down the left tree, if 1, move it down the right tree. If we hit a null-term then we simply continue reading.
            switch(buffer[i]){
                case '0':
                    ptr = ptr->left;
                    break;
                case '1':
                    ptr = ptr->right;
                    break;
                case '\0':
                    break;
                default:
                    printf("Fatal Error: .hcz file contains invalid characters\n");
                    return -4;
                    break;
            }
            
            //If the pointer is at a leaf node, then we have decoded a word. Write this word to the output and reset the pointer
            if(ptr->left == NULL && ptr->right == NULL){
                writeString(output, ptr->string);
                ptr = head;
            }
        }
    } while (bytesRead >0);

    if(ptr != head){ //there was leftover or extra bits, caused by changed encryption, changed codebook, or incorrect codebook. Print fatal error
        printf("Fatal Error: Codebook did not line up with encryption. Removing output file.\n");
        return -1;
    }

    return 0;
}