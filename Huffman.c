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
int performOperation (int mode, Node** headAVL, int codeBook, char* inputPath, char** escapeChar);  //Will decide whether to build, compress, or decomp based on flag
int buildHuffmanCodebook(int input, Node** head, char** escapeChar);                               //Will add to AVL tree based on input files
void exportHuffman(Node* head, char** escapeChar);                                                  //Will output HuffmanCodebook based on built AVL
int recursiveOperation(char* path, int codebook, Node** head, char** escapeChar, int flagMode);     //Will call performOperation on every file in directory and recurse on every directory
/* TO DO LIST:::::::::::

    FINAL TO DO LIST
    - fix output name for decomp (uncomment it)
    - specify recusrions will recurse on huffmancodebook file in read me 
    - if recursivve call but file instead of path, do command on file and then output normally with warning. specify this in read me
    - sift up (efficeincy)
    - If (*headAVL == NULL) codebookAVL(..) _>>>> In perform operation, that way we dont have to rebuild the AVL tree everytime

*/

int main(int argc, char* argv[]){
    //Set globals. Tokens represents number of AVL tokens. Recursive is a flag
    tokens = 0, recursive = 0;

    //Check for valid inputs. Quit if invalid
    if(argc < 3) errorPrint("Fatal Error: Not enough arguments", 1);

    //each flag corresponds to 1 or 0. flagCheck will set the proper flags. If there is an error in the input, flagCheck will display error and exit()
    int bcdFlag = flagCheck(argc, argv);
    
    if(!recursive){
        int test = open(argv[2], O_RDONLY);
        if(test < 0) errorPrint("Fatal Error: Could not open file (does it exist?)", 1);
        close(test);
    }

    //
    //The input is proper in terms of number of arguments and position of each argument. Now try to open the needed items (directory/file/codebook)
    //

    Node* head = NULL;      //AVL head. Storing inputs for buildTree

    char* escapeChar = (char*) malloc(sizeof(char)*2);              //Create an escapeChar
    escapeChar[0] = _ESCAPECHAR;
    escapeChar[1] = '\0';

    int count = 0;                                                  //Create a count. This will check how many files have had an operation performed 

    //Open the codebook if we are not in build mode
    int codeBook;
    if(bcdFlag != _BUILD) codeBook = open(argv[3+recursive], O_RDONLY);
    else codeBook = -1;

    if(recursive){
        count = recursiveOperation(argv[3], codeBook, &head, &escapeChar, bcdFlag); //Recursive operation returns the number of files operated on
        if(count == -3){
            free(escapeChar);
            if(codeBook > 0) close(codeBook);
            errorPrint("Fatal Error: Could not open directory (does it exist?)", 1);
        }
    }else {
        count = performOperation(bcdFlag, &head, codeBook, argv[2], &escapeChar);   //Returns the number of files operated on
    }

    //On error, exit and return 1;
    if(count == -1){
        if(codeBook > 0) close(codeBook);
        free(escapeChar);
        if(head != NULL) free(head);
        return 1;
    }

    //If head is not null, there is something in AVL, therefore we can make a codebook as normal.
    if(head !=NULL && bcdFlag == _BUILD){
        exportHuffman(head, &escapeChar);
        freeAvl(head);
    }

    //If we're building and head is NULL, then we could not add anything to AVL, therefore there is nothing to create a Huffman Tree for
    //Thus, we will create a blank HuffmanCodebook with escape char
    if(head == NULL && bcdFlag == _BUILD){
        remove("HuffmanCodebook");
        int output = open("HuffmanCodebook", O_WRONLY | O_CREAT, 00400);
        writeString(output, escapeChar);
        writeString(output, "\n");
        close(output);
        printf("Warning: only empty file(s)/directories detected. Empty codebook created with default escape character (\\)\n");
    
    } else if(!count){
        //Count was not incremented, therefore we didn't do any operations. Either empty files, directories, or unable to open the file. Let the user know
        printf("Warning: None of the given file(s) can be compressed or decompressed. Either they don't exist, they cannot be opened, or they are blank.\n");
    }

    //Free and return, we are done
    free(escapeChar);
    if(codeBook > 0) close(codeBook);
    return 0;
}


int performOperation (int mode, Node** headAVL, int codeBook, char* inputPath, char** escapeChar){
    //Check to make sure that the inputs are valid, i.e. proper codebook if comp/decomp, dont compress .hcz, dont decompress .hcz
    //If codebook is invalid and we wanna comp/decomp, we cant do anything. Print fatal error and quit
    //Otherwise, we will continue since there may be other files that can have the operation performed on
    if(mode != _BUILD && codeBook < 0){
        printf("Fatal Error: Could not open codebook file\n");
        return -1;
    }
    
    int inputPathLength = strlen(inputPath);
    if(mode == _DECOMPRESS && (inputPathLength < 5 || strcmp(inputPath+inputPathLength-4, ".hcz") != 0)){
        return 0; //Its not <name>.hcz, cant decompress. This check is for recursion
    } else if (mode != _DECOMPRESS && (inputPathLength > 4 && strcmp(inputPath+inputPathLength-4, ".hcz") == 0)){
        return 0; //Its .hcz, cant compress or build. This check is for recursion
    }
    
    //If we cannot open the input, return 0 since no files have been operated on
    int input = open(inputPath, O_RDONLY);
    if(input < 0) return 0;

    int inputCheck = 0; 

    //Create an outputName that we will determine based on path & what mode we're in
    char* outputName;

    switch (mode){
        case _BUILD:
            outputName = malloc(1);                             //Malloc space because we will free no matter what
            inputCheck = buildHuffmanCodebook(input, headAVL, escapeChar);   //Will fill AVL tree and escapeChar based on input
            break;
        case _COMPRESS:
            outputName = (char*) malloc(sizeof(char)*(inputPathLength+5+4));        //Adjust the name by appending .hcz
            outputName[0] = '\0';
            strcpy(outputName, inputPath);
            strcat(outputName, ".hcz");

            *headAVL = codebookAvl(codeBook, insert);                               //build AVL tree (sorted by token) based on codebook given
            
            int outputComp = open(outputName, O_WRONLY | O_CREAT, 00600);
            inputCheck = getInput(headAVL, input, NULL, outputComp, mode);                       //Write out the compressed bitstring to outputPath
            close(outputComp);
            if(inputCheck != 0) remove(outputName);
            break;
        case _DECOMPRESS:
            *headAVL = codebookAvl(codeBook, rebuildHuffman);                       //Rebuild HuffmanTree using codebook
            //CORRECT OUTPUT NAME JUST NEED TO IMPLEMENT
            /*char *outputTest = (char*) malloc(inputPathLength - 4);               //Remove the .hcz extention
            outputTest[0] ='\0';
            strncpy(outputTest, inputPath, inputPathLength-4);
            printf("\n\nTesting output string: %s\n\n", outputTest);*/
            // String manipulation to figure out output filename here
            outputName = (char*) malloc(sizeof(char)*(inputPathLength+5));
            outputName[0] = '\0';
            strcpy(outputName, inputPath);
            strcat(outputName, ".txt");        //THIS NEEDS TO BE CHANGED BEFORE WE SUBMIT/////////////////////////////////////////////////////

            
            int outputDecomp = open(outputName, O_WRONLY | O_CREAT, 00600);         
            inputCheck = decompressFile(*headAVL, input, outputDecomp);                          //Write decompressed strings to output
            // close and free
            close(outputDecomp);
            if(inputCheck == -1 || inputCheck == -3 || inputCheck == -4) remove(outputName);    //These are fatal errors in decompress
            break;
        default:
            break;
    }

    //Free AVL stuff if we are not in build, because we do not need it anymore
    if(mode == _COMPRESS || mode== _DECOMPRESS){
        if(headAVL != NULL && *headAVL!= NULL) freeAvl(*headAVL);
        *headAVL = NULL;
    }

    close(input);
    free(outputName);
    if(inputCheck != 0) return -1;
    //We have succesfully performed 1 operation
    return 1;
}

int recursiveOperation(char* path, int codebook, Node** head, char** escapeChar, int flagMode){
    int count = 0, check = 0;;
    DIR* directory = opendir(path);
    if(directory == NULL){
        return -3;
    }
    readdir(directory);
    readdir(directory); //Get rid of the . and .. directories
    
    struct dirent* dir;

    //We are going to add the / in each iteration of the loop, so make sure that the input does not have the /
    //This ensures that the program will run with "<directory>" and "<directory>/"
    int pathLength = strlen(path);
    if(path[pathLength-1] == '/'){
        path[pathLength-1] = '\0';
        pathLength--;
    }

    //While there is still stuff in the directory
    while((dir = readdir(directory)) != NULL){
        if(dir->d_type != DT_DIR && dir->d_type != DT_REG) continue;                        //If it is not a file or directory, then ignore

        char* newPath = malloc(pathLength + strlen(dir->d_name)+2);                         //Figure out the path of the file based on d_name && path
        *newPath = '\0';
        strcpy(newPath, path);
        strcat(newPath, "/");
        strcat(newPath, dir->d_name);

        //If it is a directory, perform the same operations on everything inside the directory
        //If it is a file, perform the operation on the file
        //On error, close everythign and return -1
        switch (dir->d_type){
            case DT_DIR:
                check=recursiveOperation(newPath, codebook, head, escapeChar, flagMode);
                break;

            case DT_REG:
                check=performOperation(flagMode, head, codebook, newPath, escapeChar);
                lseek(codebook, 0, SEEK_SET);
                break;
            default:
                break;
        }
        
        if(check == -1){
            free(newPath);
            closedir(directory);
            return -1;
        } else {
            count += check;
        }

        free(newPath); //We are done with this path and we will malloc another new path in the next iteration
    }

    closedir(directory);
    
    //Return the number of files that have had an operation performed
    return count;

}

int buildHuffmanCodebook(int input, Node** headAVL, char** escapeChar){
    Node* head= *headAVL;

    int inputCheck; 
    inputCheck = getInput(&head, input, escapeChar, 0, _BUILD);         //This function reads the file && fills AVL with token/frequencies
    
    *headAVL = head;        //Return head by changing double pointer

    if(inputCheck != 0){
        printf("FATAL ERROR: Could not fully finish tree\n");  //inputCheck will return non-0 value if something goes wrong
        return -1;
    }
    return 0;
}

void exportHuffman(Node* head, char** escapeChar){

    //Build the heap
    HeapSize = tokens;
    treeNode* minHeap[HeapSize];
    fillMinHeapArray(minHeap, head, 0); //Turns AVL Nodes into treeNodes and puts in array (in order traversal of AVL)
    
    //Heapify the array
    int i;
    for(i = tokens -1; i >= 0; i --){
        heapify(minHeap, i);
    }

    if(HeapSize == 1){  //1 Token read
        treeNode* solo = pop(minHeap);
        treeNode* newRoot = merge(solo, NULL);
        insertHeap(minHeap, newRoot); 
    } else {
        //Huffman Algorithm. Take the 2 lowest frequencies, combine them into a tree, put the tree back in heap
        while(HeapSize >1){
            treeNode* less = pop(minHeap);
            treeNode* great = pop(minHeap);
            treeNode* newNode = merge(less, great);
            insertHeap(minHeap, newNode);
        }
    }
    //The minheap now has only 1 Node inside it, which is the Huffman Tree

    //Remove old book to prevent overwrite issues. Create new book and write the HuffmanCodebook using 
    remove("./HuffmanCodebook"); //Will not crash program if the file does not exist
    int book = open("./HuffmanCodebook", O_WRONLY | O_CREAT, 00400);
    
    //Write the escape char first, then write out the Huffman
    writeString(book, *escapeChar);
    writeString(book, "\n");
    writeCodebook(minHeap[0], book, *escapeChar, "");

    //We do not need the Huffman Tree anymore, since output is complete. Start freeing things
    freeHuff(minHeap[0]);
    close(book);
}