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

void * sizeChecker(int actualSize, BYTE * nextVal,BYTE * prevVal){
    void *addr;
    BYTE *temp;
    int numPages;
    if(actualSize<=4096){
        cout<<"we are here 1"<<endl;
            addr = sbrk(4096);
            //to store the info of the chunk in the dynamic allocated mem, have to typeCase it
            chunkinfo *ptr = (chunkinfo *)addr;
            //assign the chunk info here. the size of chunk info is 24 bytes
            ptr->inuse = 1;     //since only being allocated and not in_use
            ptr->size = 4096;
            ptr->next = nextVal;
            ptr->prev = prevVal;
            //end of the chunk info
            
            //to offset to actual data:
            
            temp = (BYTE *)addr + sizeof(chunkinfo);
            addr = (void*)temp;

            //return the ptr with offset of 24 bytes
            return(addr);
        }
        else if((actualSize%4096)==0){

            cout<<"we are here 2"<<endl;
            numPages = (actualSize/4096);

            addr = sbrk(4096*numPages);
            //to store the info of the chunk in the dynamic allocated mem, have to typeCase it
            chunkinfo *ptr = (chunkinfo *)addr;
            //assign the chunk info here. the size of chunk info is 24 bytes
            ptr->inuse = 1;     //since only being allocated and not in_use
            ptr->size = 4096*numPages;
            ptr->next = nextVal;
            ptr->prev = prevVal;
            //end of the chunk info

            //to offset to actual data:
            
            temp = (BYTE *)addr + sizeof(chunkinfo);
            addr = (void*)temp;

            //return the ptr with offset of 24 bytes
            return(addr);
        }
        else{
            cout<<"we are here 3!"<<endl;
            numPages = (actualSize/4096)+1;

            addr = sbrk(4096*numPages);
            //to store the info of the chunk in the dynamic allocated mem, have to typeCase it
            chunkinfo *ptr = (chunkinfo *)addr;
            //assign the chunk info here. the size of chunk info is 24 bytes
            ptr->inuse = 1;     //since only being allocated and not in_use
            ptr->size = 4096*numPages;
            ptr->next = nextVal;
            ptr->prev = prevVal;
            //end of the chunk info

            //to offset to actual data:
            
            temp = (BYTE *)addr + sizeof(chunkinfo);
            addr = (void*)temp;

            //return the ptr with offset of 24 bytes
            return(addr);
        }
}



void *myMalloc(int size){    //accepts the size of the memory to be allocated
    void *addr;
    BYTE * temp;
    int actualSize = size+ sizeof(chunkinfo);
    if(startOfHeap==NULL){  //if no heap then create the chunk requested by the user
        addr = sizeChecker(actualSize,NULL,NULL);
        //set the start,end of the heap to this chunk.
        temp = (BYTE*)addr - sizeof(chunkinfo); //for startOfHeap and EndOfHeap we need to offset it to headerInfo
        startOfHeap = temp;
        EndOfHeap = temp;
        return addr;
    }
    else{   //if heap is present then
        chunkinfo *curr = (chunkinfo *)startOfHeap;

        if(curr->inuse != 1 && actualSize <= curr->size){
            curr->inuse = 1;
            //curr->size = ?    //not sure what to do with the splitting, we'll see
            addr = (void *)curr;

            //to offset to actual data:
            
            temp = (BYTE *)addr + sizeof(chunkinfo);
            addr = (void*)temp;

            return(addr);
        }
        else{
            while(curr->next !=NULL){
                curr = (chunkinfo*)curr->next;
                    if(curr->inuse != 1 && actualSize <= curr->size){
                        curr->inuse = 1;
                        //curr->size = ?    //not sure what to do with the splitting, we'll see
                        addr = (void *)curr;

                        //to offset to actual data:
                        
                        temp = (BYTE *)addr + sizeof(chunkinfo);
                        addr = (void*)temp;

                        return(addr);
                        break;
                }
            }
            //if we traverse through the whole list and still did not find any page to insert, then create new node at the end
            addr = sizeChecker(actualSize,NULL,(BYTE*)curr); //allocate new mem at the end of the page
            curr->next = (BYTE*)addr-sizeof(chunkinfo);   //set the last page next add to new allocated mem

            temp = (BYTE*)addr - sizeof(chunkinfo); //for startOfHeap and EndOfHeap we need to offset it to headerInfo

            EndOfHeap = temp;
            return addr;
        }
    
    }
}


void analyze(){
    printf("\n--------------------------------------------------------------\n");
    if(startOfHeap==NULL)
    {
    printf("no heap \n");
    return;
    }
    chunkinfo* ch = (chunkinfo*)startOfHeap;
    for (int no=0; ch; ch = (chunkinfo*)ch->next,no++)
    {
    printf("%d | current addr: %x |", no, ch);
    printf("size: %d | ", ch->size);
    printf("info: %d | ", ch->inuse);
    printf("next: %x | ", ch->next);
    printf("prev: %x", ch->prev);
    printf(" \n");
    }
    printf("program break on address: %x\n",sbrk(0));
}



int main(){
    analyze();
    void * addr1,*addr2,*addr3,*addr4;

    addr1 = myMalloc(54);
     addr2 = myMalloc(8168);
     addr3 = myMalloc(8333);

    addr4 = myMalloc(2348); 
    analyze();



    return 0;
}