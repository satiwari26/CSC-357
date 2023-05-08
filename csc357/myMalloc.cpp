#include <unistd.h>
#include <stdlib.h>
#include<stdio.h>

typedef unsigned int BYTE; //for 32 bit system, guessing the size is 4 byte

struct chunkinfo
{
int size; //size of the complete chunk including the chunkinfo
int inuse; //is it free? 0 or occupied? 1
BYTE *next,*prev;//address of next and prev chunk
};


int main(){

    return 0;
}