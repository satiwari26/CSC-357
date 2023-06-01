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


int main(int argc, char *argv[]){ 
    if(argc !=4){
        printf("%s \n", "not the right number of arguments");
        return 0;
    }

    int par_count = atoi(argv[3]);
    if(par_count<1){
        par_count = 1;
    }
    else if(par_count>10){
        par_count = 10;
    }
    //keep track of the par_pids
    int *par_pid = (int *)mmap(NULL,par_count*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    for(int i=0;i<par_count;i++){
        int pid = fork();
        if(pid== -1){
            printf("somethign wrong!\n");
            perror("fork\n");
            exit(1);
        }
        else if(pid==0){
            par_pid[i] = i;

            char *args[4];
            args[0] = (char*)malloc(100);
            args[1] = (char*)malloc(100);
            args[2] = (char*)malloc(100);
            args[3] = (char*)malloc(100);

            strcpy(args[0],argv[2]);
            snprintf(args[1], sizeof(args[1]), "%d", par_pid[i]);
            snprintf(args[2], sizeof(args[2]), "%d", par_count);   
            args[3] = NULL;

            fflush(stdout);

            execv(args[0],args);

            printf("should never get to this point?\n");

            exit(0);    //should never get to this point
        }
    }

    wait(0);
    // fflush(stdout);
    return 0;
}