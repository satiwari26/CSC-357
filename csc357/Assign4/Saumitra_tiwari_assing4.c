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


int main(){
    int par_count = 1; 
    //keep track of the par_pids
    int *par_pid = (int *)mmap(NULL,par_count*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    for(int i=0;i<par_count;i++){
        if(fork()==0){
            par_pid[i] = i;

            char *args[4];
            args[0] = (char*)malloc(100);
            args[1] = (char*)malloc(100);
            args[2] = (char*)malloc(100);
            args[3] = (char*)malloc(100);

            strcpy(args[0],"matrix.c");
            snprintf(args[1], sizeof(args[1]), "%c", par_pid[0]);
            snprintf(args[2], sizeof(args[2]), "%c", par_count);   
            args[3] = NULL;
            execv("./matrix",args);

            exit(0);
        }
    }
    
    return 0;
}