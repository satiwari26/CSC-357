#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){

    char * phraseArray = (char*)mmap(NULL,1000*sizeof(char),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    char thred1[] = "my name is Cruel hero. This is a test that performs the operation of the mutex. Here two process will go into the state of the fighting each other.";
    char thred2[] = "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

    if(fork()==0){
        for(int i=0;;i++){
            if(i%2==0){
                strcpy(phraseArray,thred1);
            }
            else{
                strcpy(phraseArray,thred2);
            }
        }
    }

    while(true){
        char parentarray[1000];
        strcpy(parentarray,phraseArray);
        printf("%s \n",parentarray);
    }

    wait(0);    //wait for all the kids to finish the process

    return 0;
}