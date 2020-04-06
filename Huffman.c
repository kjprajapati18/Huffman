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
void performOperation (int mode, Node** headAVL, int codeBook, char* inputPath, char** escapeChar);
void buildHuffmanCodebook(int input, Node** head, char** escapeChar);
void exportHuffman(Node* head, char** escapeChar);
int recursiveOperation(char* path, int codebook, Node** head, char** escapeChar, int flagMode);
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

    FINAL TO DO LIST
    - fix output name for decomp
    - test case for empty file empty directory (i say warning and do it anyway)
    - free everything during errors
        1. working case it frees everyting
        2. every errro case it frees everything
    - comment code and remove random nonsesnse
    - specify recusrions will recurse on huffmancodebook file in read me 
    - fix order of recursion flag ?? maybe i think a warning if fligpped
    - check codebook format
    - if recursivve call but file instead of path, do command on file and then output normally with warning. specify this in read me
    - sift up (efficeincy)

*/

int main(int argc, char* argv[]){
    tokens = 0, recursive = 0;

    //Check for valid inputs
    if(argc < 3) errorPrint("Fatal Error: Not enough arguments", 1);

    //each flag corresponds to 1 or 0. flagCheck will set the proper flags. If there is an error in the input, flagCheck will display error and exit()
    int bcdFlag = flagCheck(argc, argv);
    
    //The input is proper in terms of number of arguments and position of each argument. Now try to open the needed items (directory/file/codebook)
    
    Node* head = NULL;      //AVL head. Storing inputs for buildTree

    char* escapeChar = (char*) malloc(sizeof(char)*2);
    escapeChar[0] = _ESCAPECHAR;
    escapeChar[1] = '\0';

    int codeBook;
    if(bcdFlag != _BUILD) codeBook = open(argv[3+recursive], O_RDONLY);
    else codeBook = -1;

    if(recursive){
        /*DIR* directory = opendir(argv[3]);
        if(directory == NULL) errorPrint("Could not open directory", 1);
        
        printf("Printing Directories:\n\n");
        printf("Files in %s:\n", argv[3]);
        printFiles(directory, argv[3]);*/
        
        recursiveOperation(argv[3], codeBook, &head, &escapeChar, bcdFlag);

    }else {
        performOperation(bcdFlag, &head, codeBook, argv[2], &escapeChar);
    }

    if(head !=NULL && bcdFlag == _BUILD){
        exportHuffman(head, &escapeChar);
    }

    if(head == NULL){
        printf("Warning: None of the given file(s) can be opened\n");
    }else{
        //print2DTree(head, 0);
        freeAvl(head);
    }
    
    free(escapeChar);
    return 0;
}


void performOperation (int mode, Node** headAVL, int codeBook, char* inputPath, char** escapeChar){
    if(mode != _BUILD && codeBook < 0) errorPrint("Could not open codebook file", 1);
    
    int inputPathLength = strlen(inputPath);
    if(mode == _DECOMPRESS && (inputPathLength < 5 || strcmp(inputPath+inputPathLength-4, ".hcz") != 0)){
        return; //Its not <name>.hcz, cant decompress. This check is for recursion
    } else if (mode != _DECOMPRESS && (inputPathLength > 4 && strcmp(inputPath+inputPathLength-4, ".hcz") == 0)){
        return; //Its .hcz, cant compress or build. This check is for recursion
    }
    
    int input = open(inputPath, O_RDONLY);
    if(input < 0) return;


    char* outputName;

    switch (mode){
        case _BUILD:
            outputName = malloc(1); //Malloc space because we will free no matter what
            buildHuffmanCodebook(input, headAVL, escapeChar);
            break;
        case _COMPRESS:
            outputName = (char*) malloc(sizeof(char)*(inputPathLength+5+4));
            outputName[0] = '\0';
            strcpy(outputName, inputPath);
            strcat(outputName, ".hcz");

            *headAVL = codebookAvl(codeBook, insert);
            //print2DTree(*headAVL, 0);
            int outputComp = open(outputName, O_WRONLY | O_CREAT, 00600);
            getInput(headAVL, input, NULL, outputComp, mode);

            close(outputComp);
            break;
        case _DECOMPRESS:
            *headAVL = codebookAvl(codeBook, rebuildHuffman); 
            // String manipulation to figure out output filename here
            outputName = (char*) malloc(sizeof(char)*(inputPathLength+5));
            outputName[0] = '\0';
            strcpy(outputName, inputPath);
            strcat(outputName, ".txt");         //THIS NEEDS TO BE CHANGED BEFORE WE SUBMIT/////////////////////////////////////////////////////

            // Here is going to be writing the decompress function (new function)
            int outputDecomp = open(outputName, O_WRONLY | O_CREAT, 00600);
            //if(*(headAVL->string) == ' ') printf("\n\nYEA\n\n", headAVL->string);
            //print2DTree(headAVL, 0);
            decompressFile(*headAVL, input, outputDecomp);
            // close and free
            close(outputDecomp);
            break;
        default:
            break;
    }
    
    close(input);
    free(outputName);
}

int recursiveOperation(char* path, int codebook, Node** head, char** escapeChar, int flagMode){
    DIR* directory = opendir(path);
    readdir(directory);
    readdir(directory); //Get rid of the . and .. directories
    
    struct dirent* dir;
    int pathLength = strlen(path);
    if(path[pathLength-1] == '/'){
        path[pathLength-1] = '\0';
        pathLength--;
    }


    while((dir = readdir(directory)) != NULL){
        if(dir->d_type != DT_DIR && dir->d_type != DT_REG) continue;

        char* newPath = malloc(pathLength + strlen(dir->d_name)+2);
        *newPath = '\0';
        strcpy(newPath, path);
        strcat(newPath, "/");
        strcat(newPath, dir->d_name);
        printf("PathName: %s\n", newPath);

        switch (dir->d_type){
            case DT_DIR:
                recursiveOperation(newPath, codebook, head, escapeChar, flagMode);
                break;

            case DT_REG:
                performOperation(flagMode, head, codebook, newPath, escapeChar);
                lseek(codebook, 0, SEEK_SET);
                break;
            default:
                break;
        }
        free(newPath);
    }

    closedir(directory);
    return 0;

}

void buildHuffmanCodebook(int input, Node** headAVL, char** escapeChar){
    Node* head= *headAVL;

    // char* escapeChar = (char*) malloc(sizeof(char)*2);
    // escapeChar[0] = _ESCAPECHAR;
    // escapeChar[1] = '\0';

    int inputCheck; 
    inputCheck = getInput(&head, input, escapeChar, 0, _BUILD);
    if(inputCheck != 0) errorPrint("FATAL ERROR: Could not fully finish tree", 1); //expand to different errors, but also make a different function to handle the differnt error
    
    *headAVL = head;
}

void exportHuffman(Node* head, char** escapeChar){

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
    writeString(book, *escapeChar);
    writeString(book, "\n");
    writeCodebook(minHeap[0], book, *escapeChar, "");

    //freeAvl(head);
    //free(escapeChar);             /////// PUT THESE BACK
    freeHuff(minHeap[0]);
    close(book);
}