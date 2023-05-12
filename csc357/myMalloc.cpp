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

void splitPages(chunkinfo *curr, int pageSize){ //splits the page
    BYTE * temp;
    temp = (BYTE*)curr->next;   //storing the current next node val
    curr->next = (BYTE*)curr+(pageSize); //offset it to the unused chunk
    curr->inuse = 1;
    //set the next chunk info
    chunkinfo *splitNode = (chunkinfo*)curr->next;
    splitNode->next = temp;
    splitNode->prev = (BYTE*)curr;
    splitNode->inuse = 0;
    splitNode->size = curr->size-pageSize;
    //now setting the curr size to pageSize
    curr->size = pageSize;
    //update the next page prev to new split page
    curr = (chunkinfo*)splitNode->next;
    curr->prev = (BYTE*)splitNode;
}



void *myMalloc(int size){    //accepts the size of the memory to be allocated
    void *addr;
    BYTE * temp;
    int pageSize =0;    //to calculate the page size
    int actualSize = size+ sizeof(chunkinfo);
    
    chunkinfo * suitableAddr = NULL;    //best page to store the requested data...

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
        startOfHeap = (void *)temp;
        EndOfHeap = (void *)temp;
        return addr;
    }
    else{   //if heap is present then
        chunkinfo *curr = (chunkinfo *)startOfHeap;
        
            while(curr != NULL){
                    if(curr->inuse != 1 && pageSize <= curr->size){
                        if(pageSize==curr->size){   //if exactly equal and not in use
                            curr->inuse = 1;

                            // addr = (void *)curr;
                            // //to offset to actual data:
                            // temp = (BYTE *)addr + sizeof(chunkinfo);
                            // addr = (void*)temp;

                            // return(addr);
                            suitableAddr = curr;
                             break;
                        }
                        else{   
                            if(suitableAddr == NULL){
                                suitableAddr = curr;
                            }
                            else if(curr->size<suitableAddr->size){
                                suitableAddr = curr;
                            }
                        }
                    }
                    temp = (BYTE*)curr;
                    curr = (chunkinfo*)curr->next;  //move to the next page
                }
            
            if(suitableAddr == NULL){
            //if we traverse through the whole list and still did not find any page to insert, then create new node at the end
            addr = sizeChecker(actualSize,NULL,temp); //allocate new mem at the end of the page
            suitableAddr = (chunkinfo*)temp;
            suitableAddr->next = (BYTE*)addr-sizeof(chunkinfo);   //set the last page next add to new allocated mem

            temp = (BYTE*)addr - sizeof(chunkinfo); //for startOfHeap and EndOfHeap we need to offset it to headerInfo

            EndOfHeap = temp;
            return addr;
            }
            else{
                if(suitableAddr->size == pageSize){
                    addr = (void *)suitableAddr;
                    //to offset to actual data:
                    temp = (BYTE *)addr + sizeof(chunkinfo);
                    addr = (void*)temp;

                    return(addr);
                }
                splitPages(suitableAddr,pageSize);

                addr = (void *)suitableAddr;
                    //to offset to actual data:
                    temp = (BYTE *)addr + sizeof(chunkinfo);
                    addr = (void*)temp;

                    return(addr);
            }
    }   
}

void myFree(void *inputVal){ 

    chunkinfo *addrVal = (chunkinfo*)inputVal;
    if(startOfHeap==NULL){
        cout<<"Error! there is no allocated memory to remove."<<endl;
        return;
    }

    addrVal = addrVal-1;    //offset it to 24 bytes back!!
    //collect info of prev and next node
    chunkinfo *nextVal,*prevVal,*nextNextVal;
    nextVal = (chunkinfo*)addrVal->next;
    prevVal = (chunkinfo*)addrVal->prev;
    if(addrVal->next !=NULL){
        if(nextVal->next !=NULL){
        nextNextVal = (chunkinfo*)nextVal->next;
        }
        else{
        nextNextVal = NULL;
        }
    }
    else{
        nextNextVal = NULL;
    }

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
            if(addrVal ==(chunkinfo*)startOfHeap){  //if we remove the first element after inserting other elements
                addrVal->inuse = 0;
                if(nextVal->inuse ==0){
                    addrVal->next = nextVal->next;
                    addrVal->size = addrVal->size+nextVal->size;
                }
                return;
            }

        if(nextVal->inuse==0 && prevVal->inuse==0){  //if prev and next pages are empty
            prevVal->next = nextVal->next;
            if(nextNextVal !=NULL){
            nextNextVal->prev = addrVal->prev;
            }
            prevVal->size = (prevVal->size)+(addrVal->size)+(nextVal->size);
        }
        else if(prevVal->inuse==0){ //if only prev page is empty
            prevVal->next = addrVal->next;
            nextVal->prev = addrVal->prev;
            prevVal->size = (prevVal->size)+(addrVal->size);
        }
        else if(nextVal->inuse==0){   //if nextVal page is empty
            addrVal->next = nextVal->next;
            if(nextNextVal !=NULL){
            nextNextVal->prev = (BYTE*)addrVal;
            }
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

void timedTest()
{
    void *a[100];
    clock_t ca, cb;
    auto average = 0;
    auto iterations = 1000;
    double duration_microseconds = 0;
    for (int i = 0; i < iterations; i++)
    {
        ca = clock();

        for (int i = 0; i < 100; i++)
            a[i] = myMalloc(1000);
        for (int i = 0; i < 90; i++)
            myFree(a[i]);
        myFree(a[95]);
        a[95] = myMalloc(100);
        for (int i = 90; i < 100; i++)
            myFree(a[i]);

        cb = clock();
        duration_microseconds = ((double)(cb - ca)) / CLOCKS_PER_SEC *1000000;
        average += duration_microseconds;
    }
    printf("\nAverage duration: %f microseconds\n", (double)(average) / iterations);
    // printf("\nduration: %f microseconds\n", (double)(cb - ca));
}

void bestFitSplitTest()
{
    void *a[100];
    a[0] = myMalloc(4090);
    a[1] = myMalloc(8180);
    a[2] = myMalloc(4090);
    a[3] = myMalloc(4090);
    a[4] = myMalloc(4090);
    analyze();
    myFree(a[1]);
    analyze();
    myFree(a[3]);
    analyze();
    a[5] = myMalloc(1000);
    analyze();
    a[6] = myMalloc(1000);
    analyze();
    myFree(a[4]);
    analyze();
}

int main(){

    bestFitSplitTest();

    // void * a[20];
    // for(int i=0;i<10;i++){
    //     a[i] = myMalloc(100);
    // }
    // analyze();

    // for(int j=0;j<10;j++){
    //     myFree(a[j]);
    //     analyze();
    // }

    // void*a[100];
    // clock_t ca, cb;
    // ca = clock();
    // for(int i=0;i<100;i++)
    // a[i]= myMalloc(1000);
    // for(int i=0;i<90;i++)
    // myFree(a[i]);
    // myFree(a[95]);
    // a[95] = myMalloc(1000);
    // for(int i=90;i<100;i++)
    // myFree(a[i]);
    // cb = clock();
    // printf("\nduration: % f\n", ((double)(cb - ca))/ CLOCKS_PER_SEC *1000000);

    // //timedTest();

    return 0;
}