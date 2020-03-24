#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flagCheck(int flag, int argc, char* argv[]);

int main(int argc, char* argv[]){
    flagCheck(1, argc, argv);
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
    flagCheck(pos+1, argc, argv);

}