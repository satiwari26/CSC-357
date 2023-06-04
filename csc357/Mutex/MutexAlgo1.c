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

int main(){ //peterson's algorithm fir critical section

    char * phraseArray = (char*)mmap(NULL,1000*sizeof(char),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    bool * processFlag = (bool*)mmap(NULL,2*sizeof(bool),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    //flag[0] corresponds to child
    //flag[1] to parent
    int * processTurn = (int*)mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    //0 corresponds to child process turn
    //1 corresponds to parent process turn

    char thred1[] = "my name is Cruel hero. This is a test that performs the operation of the mutex. Here two process will go into the state of the fighting each other. \n";
    char thred2[] = "--------------------------------------------------------------------------------------------------------------------------------------------------- \n";

    int Childpid = fork();
    if(Childpid ==0){
        for(int i=0;;i++){
            if(i%2==0){
                do{
                    processFlag[0] = 1; //set the flag to 1 to let the other process know that it is ready to hit the critical section
                    *processTurn = 1;    //humble set the turn to parent process
                }
                while(processFlag[1] && *processTurn==1);
                strcpy(phraseArray,thred1); //critical section.
                processFlag[0] = 0; //set the process flag to 0 after getting over the critical section
            }
            else{
                do{
                    processFlag[0] = 1; //set the flag to 1 to let the other process know that it is ready to hit the critical section
                    *processTurn = 1;    //humble set the turn to parent process
                }
                while(processFlag[1] && *processTurn==1);
                strcpy(phraseArray,thred2); //critical section.
                processFlag[0] = 0; //set the process flag to 0 after getting over the critical section            }
        }
    }
    }
    FILE *file = fopen("CriticalSection.txt","w");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    while(true){
        char parentarray[1000];
        do{
            usleep(50); //adding 50 microsecond delay to let child go first
            processFlag[1] = 1; //make parent process ready
            *processTurn = 0;    //humble give turn to the child process
        }
        while(processFlag[0] && *processTurn == 0);  //wait for the child process to exit the condition

        strcpy(parentarray,phraseArray);    //critical section

        processFlag[1] = 0; //make the parent process unready again

        fprintf(file, parentarray);
        printf("%s \n",parentarray);
    }

    fclose(file);
    kill(Childpid,SIGTERM); //kill the child right as I stop the process

    wait(0);    //wait for all the kids to finish the process

    return 0;
}