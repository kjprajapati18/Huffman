#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "avl.h"
/* TO DO LIST:::::::::::

    -Add checks for every malloc                                        (cuz he's reading our code)
    -Make insert on AVL tree return an int and use double pointer       (I think that's convention)
    -Rename LR and RR to leftRotation and rightRotation                 (for clarity)
    -FIx AVL inserts
    -Fix getInput spacing issue

*/
int flagCheck(char* argv[]);
void printFiles(DIR* directory, char* path);
void errorPrint(const char* message, int exitCode);
int fillAVL(Node** head, int fd);
void print2DTree(Node* root, int space);
int recursive = 0, build = 0, compress = 0, decomp = 0;


int main(int argc, char* argv[]){

    Node* head = NULL;

    //Check for valid inputs
    if(argc < 3) errorPrint("Fatal Error: Not enough arguments", 1);

    //each flag corresponds to 1 or 0, flags will add up all flags except for the recursive flag
    int flags = flagCheck(argv);
    if(flags != 1) errorPrint("Fatal Error: Invalid flag usage. Make sure to pick 1 flag from the following: (b)uild, (c)ompress, (d)ecompress", 1);

    //build should have 3 arguments and compress/decompress should have 4 flags (+1 if recursive flag included)
    if((build && argc != 3+recursive) || ((compress + decomp) && argc != 4+recursive)) errorPrint("Fatal Error: Incorrect number of arguments for given flags", 1);

    //Make sure that a proper codebook is given
    if((compress + decomp) && strcmp("HuffmanCodebook", argv[3+recursive])) errorPrint("Fatal Error: The codebook should be called 'HuffmanCodebook.'", 1);

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

    //Finish up later
    int input = open(argv[2], O_RDONLY);
    if(input < 0) errorPrint("Could not open input file", 1);
    int inputCheck = fillAVL(&head, input);
    if(inputCheck == -1) errorPrint("FATAL ERROR: Could not fully finish tree", 1);
    
    print2DTree(head, 0);
    
    if(compress + decomp){
        int codebook = open(argv[3], O_RDONLY);
        if(codebook < 0) errorPrint("Could not open codebook", 1);
    }
    return 0;
}

//checks the first 2 flags to see what we're doing and if it's recursive or not
int flagCheck(char* argv[]){
    int pos;

    for(pos = 1; pos < 3; pos++){
        if(*argv[pos] == '-'){
            //Check which flag
            switch (*(argv[pos]+1)){
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

    return build + compress + decomp;
}

//prints all files in a given directory and all subdirectories
void printFiles(DIR* directory, char* basePath){
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
}

//Make printing error messages a little cleaner
void errorPrint(const char* message, int exitCode){
    printf("%s\n", message);
    if(exitCode != 0) exit(exitCode);
}


//Reader function to get input from files
int fillAVL(Node** head, int fd){

    Node* list = *head;
    int bytesRead = 1;
    int size = 0;
    char buffer[201];
    int i = 0;
    int broke = 0;

    //If words aren't complete by the time read returns, we need to carry the word over.
    int carryOverSize = 0;
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
	        if (buffer[i] == ' '){
                buffer[i] = '\0';

                if(carryOverSize != 0){
                    char* temp = (char*) malloc(sizeof(char)*(carryOverSize+(i-startIndex)+1));
                    memcpy(temp, carryOver, carryOverSize);
                    strcat(temp, buffer+startIndex);
                    free(carryOver);
                    carryOver = temp;
                    list = insert(list, carryOver);
                    carryOverSize = 0;
                } else {
                    list = insert(list, buffer+startIndex);
                }

                list = insert(list, " ");                   //FIX HERE::: If we at EOF, we shouldn't add a space. So theres some edge cases missing here

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
                memcpy(temp, carryOver, carryOverSize);
                strcat(temp, buffer+startIndex);
                free(carryOver);
                carryOver = temp;
            }
        }

    }while(bytesRead>0);
    
    if(carryOverSize !=0){
        insert(list, carryOver);
    }
    free(carryOver);
    *head =list;
    return 0;
  /*
  ptr = list->first;
  while(ptr != NULL){
    for(i =0; i < strlen(ptr->value); i++){
      if(isspace((ptr->value)[i])){
	char buff[strlen(ptr->value) - i];
	(ptr->value)[i] = '\0';
	strcpy(buff, ptr->value + i + 1);
	strcat(ptr->value, buff);
	i--;
      }
    }
    ptr = ptr->next;
  }

  if(broke == 1) return 0;
  return 1;*/
}


void print2DTree(Node *root, int space) 
{ 
    // Base case 
    if (root == NULL) 
        return; 
  
    // Increase distance between levels 
    space += 10; 
  
    // Process right child first 
    print2DTree(root->right, space); 
  
    // Print current node after space 
    // count 
    printf("\n"); 
    for (int i = 10; i < space; i++) 
        printf(" ");
    if(strcmp(" ", root->string) == 0) printf("[space]\n");
    else printf("%s\n", root->string); 
  
    // Process left child 
    print2DTree(root->left, space); 
}