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
    int pageSize =0;    //to calculate the page size
    int actualSize = size+ sizeof(chunkinfo);

    if(actualSize<=4096){
        pageSize = 4096;
    }
    else if(actualSize%(4096)==0){
        pageSize = (4096)*(actualSize/4096);
    }
    else{
        pageSize = (4096)*((actualSize/4096)+1);
    }

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
            if(pageSize==curr->size){
                curr->inuse = 1;
            }
            else{   //split the unused space into the required peices
                curr->inuse = 1;
                temp = (BYTE*)curr->next;   //storing the current next node val
                curr->next = (BYTE*)curr+(pageSize); //offset it to the unused chunk
                //set the next chunk info
                chunkinfo *splitNode = (chunkinfo*)curr->next;
                splitNode->next = temp;
                splitNode->prev = (BYTE*)curr;
                splitNode->inuse = 0;
                splitNode->size = curr->size-pageSize;
                //now setting the curr size to pageSize
                curr->size = pageSize;
            }
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
                        if(pageSize==curr->size){
                            curr->inuse = 1;
                        }
                        else{   //split the unused space into the required peices
                            curr->inuse = 1;
                            temp = (BYTE*)curr->next;   //storing the current next node val
                            curr->next = (BYTE*)curr+(pageSize); //offset it to the unused chunk
                            //set the next chunk info
                            chunkinfo *splitNode = (chunkinfo*)curr->next;
                            splitNode->next = temp;
                            splitNode->inuse = 0;
                            splitNode->prev = (BYTE*)curr;
                            splitNode->size = curr->size-pageSize;
                            //now setting the curr size to pageSize
                            curr->size = pageSize;
                        }
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

void myFree(chunkinfo *addrVal){ 

    if(startOfHeap==NULL){
        cout<<"Error! there is no allocated memory to remove."<<endl;
        return;
    }

    addrVal = addrVal-1;    //offset it to 24 bytes back!!
    //collect info of prev and next node
    chunkinfo *nextVal,*prevVal;
    nextVal = (chunkinfo*)addrVal->next;
    prevVal = (chunkinfo*)addrVal->prev;

    if(addrVal==(chunkinfo*)EndOfHeap){  //if the last page on the mem, move the pgrm brk up

        if(addrVal !=(chunkinfo*)startOfHeap){  //make sure that it is not the first element that we are removing
        prevVal->next = NULL;   //since we are removing the last node out
        }
        else{   //if it is the first element that we are using, set the start and the end of heap to NULL
            startOfHeap = NULL;
            EndOfHeap = NULL;
        }
        EndOfHeap = (void*)addrVal->prev;  //change the End pointer to last page addr
        sbrk(-(addrVal->size));
    }
    else{
        if(nextVal->inuse==0 && prevVal->inuse==0){  //if prev and next pages are empty
            prevVal->next = nextVal->next;
            prevVal->size = (prevVal->size)+(addrVal->size)+(nextVal->size);
        }
        else if(prevVal->inuse==0){ //if only prev page is empty
            prevVal->next = addrVal->next;
            prevVal->size = (prevVal->size)+(addrVal->size);
        }
        else if(nextVal->inuse==0){   //if nextVal page is empty
            addrVal->next = nextVal->next;
            addrVal->inuse = 0;
            addrVal->size = (addrVal->size)+(nextVal->size);
        }
        else{   //if previous and next page are not empty, then remove the current page
            addrVal->inuse = 0;
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
    void * addr1,*addr2,*addr3,*addr4,*addr5,*addr6,*addr7;

    addr1 = myMalloc(54);
     addr2 = myMalloc(8168);
     addr7 = myMalloc(24);
     myFree((chunkinfo*)addr2);
     analyze();
     addr3 = myMalloc(346);
    analyze();
    addr4 = myMalloc(43); 
    analyze();
    addr5 = myMalloc(2345);
    myFree((chunkinfo*)addr4);
    addr6 = myMalloc(9000);
    analyze();



    return 0;
}