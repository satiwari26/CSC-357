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

    char thred1[] = "my name is Cruel hero. This is a test that performs the operation of the mutex. Here two process will go into the state of the fighting each other. \n";
    char thred2[] = "--------------------------------------------------------------------------------------------------------------------------------------------------- \n";

    int Childpid = fork();
    if(Childpid ==0){
        for(int i=0;;i++){
            if(i%2==0){
                strcpy(phraseArray,thred1);
            }
            else{
                strcpy(phraseArray,thred2);
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
        strcpy(parentarray,phraseArray);
        fprintf(file, parentarray);
        printf("%s \n",parentarray);
    }

    fclose(file);
    kill(Childpid,SIGTERM); //kill the child right as I stop the process

    wait(0);    //wait for all the kids to finish the process

    return 0;
}