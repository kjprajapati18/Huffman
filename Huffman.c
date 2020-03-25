#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void flagCheck(int flag, int argc, char* argv[]);

int build = 0;
int compress = 0;
int decompress = 0;
int recur = 0;

int main(int argc, char* argv[]){
    flagCheck(1, argc, argv);
    DIR* directory = opendir(argv[3]);
    if(directory == NULL){
        printf("Could not open directory");
    }
    struct dirent *dir;
    while((dir = readdir(directory)) != NULL){
        printf("%s\n", dir->d_name);
    }
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