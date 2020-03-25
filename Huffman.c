#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void flagCheck(int flag, int argc, char* argv[]);
void printFiles(DIR* directory);

int main(int argc, char* argv[]){
    flagCheck(1, argc, argv);
    DIR* directory = opendir(argv[3]);
    if(directory == NULL){
        printf("Could not open directory");
    }
    printf("Printing Directories:\n\n");
    printf("Files in %s:\n", argv[3]);
    printFiles(directory);
    
    return 0;
}

//checks the first 2 flags to see what we're doing and if it's recursive or not
void flagCheck(int pos, int argc, char* argv[]){
    if(*argv[pos] == '-'){
        if(*(argv[pos]+1) == 'b'){
            printf("Using Build Huffman Codebook Flag\n");
        }
        else if(*(argv[pos]+1) == 'c'){
            printf("Using Compression Flag\n");
        }
        else if(*(argv[pos]+1) == 'd'){
            printf("Using the Decompression Flag\n");
        }
        else if(*(argv[pos]+1) == 'R'){
            printf("Using the Recursion Flag\n");
        }
    }
    if (pos == 1) flagCheck(pos+1, argc, argv);
}

//prints files in a given directory and all subdirectories
void printFiles(DIR* directory){
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
            DIR* direct = opendir(dir->d_name);
            printf("Files in %s: \n", dir->d_name);
            printFiles(direct);
        }
    }
}