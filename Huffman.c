#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flagCheck(int flag);

int main(int argc, char* argv[]){
    flagCheck(1);
    return 0;
}

//checks the first 2 flags to see what we're doing and if it's recursive or not
void flagCheck(int pos){
    if(*(argv[pos]) == "-"){
        if(*(argv[pos]+1) == "b"){
            printf("Using Build Huffman Codebook Flag");
        }
        else if(*(argv[pos]+1) == "c"){
            printf("Using Compression Flag")
        }
        else if(*(argv[pos]+1) == "d"){
            printf("Using the Decompression Flag");
        }
        else if(*(argv[pos]+1) == "R"){
            printf("Using the Recursion Flag");
            if(pos == 1) flagCheck(pos+1);
        }
    }
}