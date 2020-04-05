//TODO list:
//heap: insert method, remove method, array of treenode*
//tree: tree nodes: treeNodes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "avl.h"
#include "minheap.h"
#include "codebookWriter.h"
#include "inputHandler.h"
void performOperation (int mode, int codeBook, char* inputPath);
void buildHuffmanCodebook(int input);
#define _ESCAPECHAR '\\'
/* TO DO LIST:::::::::::

    -Add checks for every malloc                                        (cuz he's reading our code)
    -Make insert on AVL tree return an int and use double pointer       (I think that's convention)
    -Rename LR and RR to leftRotation and rightRotation                 (for clarity)
    
    -Rename heapify to siftDown
    -Write siftUp
    -Create insert with siftUp
    -Pop with siftDown

    -Split buildhuffman into buildhuffman and build avl

    QUESTIONS?
    -compress/decompress with blank codebook what do?
    -
*/

int main(int argc, char* argv[]){
    tokens = 0, recursive = 0;
    Node* head = NULL;      //AVL head. Storing inputs for buildTree

    //Check for valid inputs
    if(argc < 3) errorPrint("Fatal Error: Not enough arguments", 1);

    //each flag corresponds to 1 or 0. flagCheck will set the proper flags. If there is an error in the input, flagCheck will display error and exit()
    int bcdFlag = flagCheck(argc, argv);

    //The input is proper in terms of number of arguments and position of each argument. Now try to open the needed items (directory/file/codebook)
    //We can optimize this later by using +recursive on some arguments
    if(recursive){
        DIR* directory = opendir(argv[3]);
        if(directory == NULL) errorPrint("Could not open directory", 1);

        printf("Printing Directories:\n\n");
        printf("Files in %s:\n", argv[3]);
        printFiles(directory, argv[3]);
        return 0;
    }

    int codeBook;
    if(bcdFlag != _BUILD) codeBook = open(argv[3], O_RDONLY);
    else codeBook = -1;
    performOperation(bcdFlag, codeBook, argv[2]);

    return 0;
/*
    //This section is only reached if there was no recursive flag
    int input = open(argv[2], O_RDONLY);
    if(input < 0) errorPrint("Could not open input file", 1);

    char* escapeChar = (char*) malloc(sizeof(char)*2);
    escapeChar[0] = _ESCAPECHAR;
    escapeChar[1] = '\0';

    int inputCheck; 
    inputCheck = getInput(&head, input, &escapeChar, bcdFlag);
    if(inputCheck != 0) errorPrint("FATAL ERROR: Could not fully finish tree", 1); //expand to different errors, but also make a different function to handle the differnt error
    
    /*if(head !=NULL) print2DTree(head, 0);
    else printf("\nHead is null\n");
    
    //Put build huffman here
    HeapSize = tokens;
    treeNode* minHeap[HeapSize];
    fillMinHeapArray(minHeap, head, 0);
    
    int i;
    for(i = tokens -1; i >= 0; i --){
        heapify(minHeap, i);
    }                                                                                   ///////////// LETS MOVE ALL OF THIS TO A FUNCTION IN any other file
    while(HeapSize >1){
        treeNode* less = pop(minHeap);
        treeNode* great = pop(minHeap);
        treeNode* newNode = merge(less, great);
        insertHeap(minHeap, newNode);
    }
    printf("%d", HeapSize);
    print2DTreeNode(minHeap[0], 0);
    //////////////////////////
    
    remove("./HuffmanCodebook");
    int book = open("./HuffmanCodebook", O_WRONLY | O_CREAT, 00600);
    writeString(book, escapeChar);
    writeString(book, "\n");
    writeCodebook(minHeap[0], book, escapeChar, "");

    /*if(compress + decomp){
        int codebook = open(argv[3], O_RDONLY);
        if(codebook < 0) errorPrint("Could not open codebook", 1);
    }

    freeAvl(head);
    free(escapeChar);
    freeHuff(minHeap[0]);
    close(input);
    close(book);
    return 0;*/
}


void performOperation (int mode, int codeBook, char* inputPath){
    int input = open(inputPath, O_RDONLY);
    if(input < 0) errorPrint("Could not open input file", 1);
    if(mode != _BUILD && codeBook < 0) errorPrint("Could not open codebook file", 1);

    int inputPathLength = strlen(inputPath);
    char* outputName;
    Node* headAVL;

    switch (mode){
        case _BUILD:
            outputName = malloc(1); //Malloc space because we will free no matter what
            buildHuffmanCodebook(input);
            break;
        case _COMPRESS:
            outputName = (char*) malloc(sizeof(char)*(inputPathLength+5+4));
            outputName[0] = '\0';
            strcpy(outputName, inputPath);
            strcat(outputName, "test.hcz");

            headAVL = codebookAvl(codeBook, insert);
            //print2DTree(headAVL, 0);
            int outputComp = open(outputName, O_WRONLY | O_CREAT, 00600);
            getInput(&headAVL, input, NULL, outputComp, mode);

            close(outputComp);
            free(headAVL);
            break;
        case _DECOMPRESS:
            headAVL = codebookAvl(codeBook, rebuildHuffman); 
            // String manipulation to figure out output filename here
            outputName = (char*) malloc(sizeof(char)*(inputPathLength+5));
            outputName[0] = '\0';
            strcpy(outputName, inputPath);
            strcat(outputName, ".txt");         //THIS NEEDS TO BE CHANGED BEFORE WE SUBMIT/////////////////////////////////////////////////////

            // Here is going to be writing the decompress function (new function)
            int outputDecomp = open(outputName, O_WRONLY | O_CREAT, 00600);
            //if(*(headAVL->string) == ' ') printf("\n\nYEA\n\n", headAVL->string);
            //print2DTree(headAVL, 0);
            decompressFile(headAVL, input, outputDecomp);
            // close and free
            close(outputDecomp);
            free(headAVL);
            break;
        default:
            break;
    }
    
    close(input);
    free(outputName);
}

void buildHuffmanCodebook(int input){
    Node* head= NULL;

    char* escapeChar = (char*) malloc(sizeof(char)*2);
    escapeChar[0] = _ESCAPECHAR;
    escapeChar[1] = '\0';

    int inputCheck; 
    inputCheck = getInput(&head, input, &escapeChar, 0, _BUILD);
    if(inputCheck != 0) errorPrint("FATAL ERROR: Could not fully finish tree", 1); //expand to different errors, but also make a different function to handle the differnt error
    

    //Put build huffman here
    HeapSize = tokens;
    treeNode* minHeap[HeapSize];
    fillMinHeapArray(minHeap, head, 0);
    
    int i;
    for(i = tokens -1; i >= 0; i --){
        heapify(minHeap, i);
    }                                                                                   ///////////// LETS MOVE ALL OF THIS TO A FUNCTION IN any other file

    //Add the 0 case

    if(HeapSize == 1){  //1 Token read
        treeNode* solo = pop(minHeap);
        treeNode* newRoot = merge(solo, NULL);
        insertHeap(minHeap, newRoot); 
    } else {
        while(HeapSize >1){
            treeNode* less = pop(minHeap);
            treeNode* great = pop(minHeap);
            treeNode* newNode = merge(less, great);
            insertHeap(minHeap, newNode);
        }
    }

    //printf("%d", HeapSize);
    //print2DTreeNode(minHeap[0], 0);
    //////////////////////////
    
    remove("./HuffmanCodebook");
    int book = open("./HuffmanCodebook", O_WRONLY | O_CREAT, 00400);
    writeString(book, escapeChar);
    writeString(book, "\n");
    writeCodebook(minHeap[0], book, escapeChar, "");

    //freeAvl(head);
    //free(escapeChar);
    //freeHuff(minHeap[0]);
    close(book);
}

/*
int writeCodebook(treeNode* head, int fd, char* escapeChar, char* bitString){
    if(head == NULL) return -1;
    int bitLength = strlen(bitString);

    if(head->left == NULL && head->right == NULL){ //Leaf Node, add to book
        int booleanIsSpace = isspace(head->token[0]) && strcmp(head->token, " ");     //Is a control character space
        char* temp;
        int size = strlen(escapeChar);
        
        if(booleanIsSpace){
            temp = (char*) malloc((bitLength+size+4)*sizeof(char));    //bitLength(for code) + 3 (for null-term, tab, and newline) + strlen(for token)
        } else {
            temp = (char*) malloc((bitLength+strlen(head->token)+3)*sizeof(char));
        }    

        memcpy(temp, bitString, bitLength);
        temp[bitLength] = '\t';
        temp[bitLength+1] = '\0';

//        int booleanIsSpace = isspace(head->token[0]) && strcmp(head->token, " "); 

        //Just need to finish writeString function && escapeCharHandler function
        /*char* controlString = (char*) malloc(sizeof(char) * (size+2));
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
        }*//*
        
        char* inputtedToken = booleanIsSpace? escapeCharHandler(escapeChar, head->token) : head->token;

        strcat(temp, inputtedToken);
        strcat(temp, "\n");

        writeString(fd, temp);
        
        free(temp);
        if(booleanIsSpace) free(inputtedToken);
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
            //lseek(bookfd, index+1, SEEK_SET);
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
        //printf("Buffer: %s \n\n", buffer);
        /*char* word = (char*) malloc(sizeof(char));
        *word= '\0';
        char* code = (char*) malloc(sizeof(char));
        *code = '\0';*//*
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
                //printf("\n\n%s\n\n", word);
                head = treeInsert(head, word, code);
                *word = '\0';
                *code = '\0';
                startIndex = index +1;
            }
            else if(buffer[index] == '\0'){
                //printf("\n\n in carryover code\n\n");
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
*/
//checks the first 2 flags to see what we're doing and if it's recursive or not
/*int flagCheck(char* argv[]){
    int pos;

    for(pos = 1; pos < 3; pos++){       //there can only be at most 2 flags and they must be the first 2 args
        if(*argv[pos] == '-'){          //Check if its a flag
            switch (*(argv[pos]+1)){    //Check which flag and set appropriate variable
                case 'b':
                    printf("Using Build Huffman Codebook Flag\n");
                    build = 1;
                    break;
                case 'c':
                    printf("Using Compression Flag\n");
                    compress = 1;
                    break;
                case 'd':
                    printf("Using the Decompression Flag\n");
                    decomp = 1;
                    break;
                case 'R':
                    printf("Using the Recursion Flag\n");
                    recursive = 1;
                    break;
            }
        }
    }

    return build + compress + decomp;   //return the sum so that we can check if a valid flag was picked ??????????????????????????? Maybe put the checks in here for clarity (use exit)
}*/

//prints all files in a given directory and all subdirectories
/*void printFiles(DIR* directory, char* basePath){
    //this is kinda hella sus lmfao but it works
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
}*/

//Make printing error messages a little cleaner
/*void errorPrint(const char* message, int exitCode){
    printf("%s\n", message);
    if(exitCode != 0) exit(exitCode);
}*/


//Reader function to get input from files
/*int fillAVL(Node** head, int fd, char** escapeChar){

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
        bytesRead = read(fd, buffer, 200);
        if(bytesRead == -1) return -1;
        else if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';

        int startIndex = 0;
        for(i = 0; i<bytesRead; i++){
            if (buffer[i] == '\0') break;       //I don't think this line is needed but im too scared to remove
	        if (isspace(buffer[i])){
                delimiter[0] = buffer[i];
                list = insert(list, delimiter);
                buffer[i] = '\0';

                if(carryOverSize != 0){ //realloc, add tree, check/change escape char
                    carryOverSize += i-startIndex;
                    char* temp = (char*) malloc(sizeof(char)*(carryOverSize+1));
                    memcpy(temp, carryOver, carryOverSize-(i-startIndex)+1);
                    strcat(temp, buffer+startIndex);
                    free(carryOver);
                    carryOver = temp;

                    if(carryOverSize == escapeCharSize+1 && strncmp(carryOver,*escapeChar,escapeCharSize)==0){
                        incEscapeChar(escapeChar, &escapeCharSize);
                    }
                    list = insert(list, carryOver);
                    carryOverSize = 0;
                } else {
                    if(i-startIndex == escapeCharSize+1 && strncmp(buffer+startIndex,*escapeChar,escapeCharSize)==0){
                        incEscapeChar(escapeChar, &escapeCharSize);
                    }
                    list = insert(list, buffer+startIndex);
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
        if(carryOverSize == escapeCharSize+1 && strncmp(carryOver,*escapeChar,escapeCharSize)==0){
            incEscapeChar(escapeChar, &escapeCharSize);
        }
        list = insert(list, carryOver);
    }
    free(carryOver);
    *head =list;
    return 0;
}*/

/*int incEscapeChar(char** escapeChar, int* escapeCharSize){

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

}*/

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
        char* inputtedToken = booleanIsSpace? escapeCharHandler(escapeChar, head->token) : head->token;

        strcat(temp, inputtedToken);
        strcat(temp, "\n");

        writeString(fd, temp);
        free(temp);
        
        if(booleanIsSpace) free(inputtedToken);
        return 0;
    }

    //If this point is reached, then there are children. Note that Huffman trees are strictly binary
    char* newString = (char *) malloc(sizeof(char) * (bitLength+2));
    memcpy(newString, bitString, bitLength+1);
    newString[bitLength+1] = '\0';
    
    newString[bitLength] = '0';
    int stringLength = strlen(newString);
    writeCodebook(head->left, fd, escapeChar, newString);

    newString[bitLength] = '1';
    writeCodebook(head->right, fd, escapeChar, newString);

    free(newString);
    return 0;
}*/



/*int fillMinHeapArray(treeNode* minHeap[], Node* root, int count){   //Fills array with all converted TreeNode tokens from AVL
    if(root == NULL) return count;
    treeNode* newNode = (treeNode*) malloc(sizeof(treeNode));
    newNode->freq = root->val;
    newNode->token = root->string;
    newNode->left = NULL;
    newNode->right = NULL;
    minHeap[count] = newNode;
    count++;
    count = fillMinHeapArray(minHeap, root->left, count);
    count = fillMinHeapArray(minHeap, root->right, count);
    return count;
}*/

/*int writeString(int fd, char* string){

    int size = strlen(string), written = 0, status = 0;

    do{
        status = write(fd, string+written, size-written);
        written += status;
        if(status <0) return -1;
    }while(written != size);

    return 0;
}*/

/*char* escapeCharHandler(char* escapeChar, char* token){

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

}*/
