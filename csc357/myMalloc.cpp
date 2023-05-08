#include <unistd.h>
#include <stdlib.h>
#include<stdio.h>
#include <iostream>
using namespace std;

typedef unsigned char BYTE; 

struct chunkinfo
{
int size; //size of the complete chunk including the chunkinfo
int inuse; //is it free? 0 or occupied? 1
BYTE *next,*prev;//address of next and prev chunk
};

void *startOfHeap = NULL;
void *EndOfHeap = NULL;

void *myMalloc(int size){    //accepts the size of the memory to be allocated
    void *addr;
    int actualSize = size+ sizeof(chunkinfo);
    int numPages = 0;
    if(startOfHeap==NULL){  //if no heap then create the chunk requested by the user
        if(actualSize<=4096){
            addr = sbrk(4096);
            //to store the info of the chunk in the dynamic allocated mem, have to typeCase it
            chunkinfo *ptr = (chunkinfo *)addr;
            //assign the chunk info here. the size of chunk info is 24 bytes
            ptr->inuse = 0;     //since only being allocated and not in_use
            ptr->size = actualSize;
            ptr->next = NULL;
            ptr->prev = NULL;
            //end of the chunk info

            //set the start,end of the heap to this chunk.
            startOfHeap = addr;
            EndOfHeap = addr;
            //return the ptr with offset of 24 bytes
            return(addr+sizeof(chunkinfo));
        }
        else if((actualSize%4096)==0){
            numPages = (actualSize/4096);
        }
        else{
            numPages = (actualSize/4096)+1;
            
        }
    }

}


int main(){ 


    return 0;
}