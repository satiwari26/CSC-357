#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<signal.h>
#include <iostream>
#include<time.h>
#include<sys/mman.h>
#include <string.h>
#include <sys/wait.h>
using namespace std;

int fd[2];
int sigFlagChild =0;
int sigFlagParent =0;
    char text[1000];
    char activity[3000];
int save_stdin = dup(STDIN_FILENO);
//creating a signal handler function
void signalhandlerParent(int sig){
    dup2(fd[0], STDIN_FILENO);  //overwriting the stdin with fd[0]
    read(STDIN_FILENO, text,18);    //read the value in from the other end of the pipe
        dup2(save_stdin,STDIN_FILENO);
        cout<<text<<endl;
    sigFlagParent = 0;
}

void signalhandlerChild(int signal){
        // dup2(fd[0], STDIN_FILENO);  //overwriting the stdin with fd[0]
    sigFlagChild = 1;
}

int main(){
    //creating a pipe function to communicate b/w child and parent
    pipe(fd);
    int parentPID = getpid();
    int *childPID = (int *)mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    //forking into parent and child processes
    if(fork()==0){
            *childPID = getpid();
            signal(SIGUSR1,signalhandlerChild);
           while(true){
            time_t sTime = time(NULL); //start time
            int eTime = 0;

            while(eTime < 10){
                eTime = time(NULL)-sTime;
                if(sigFlagChild ==1){
                    eTime = 11;
                }
            }

            if(sigFlagChild !=1){
                close(fd[0]);   //writing end of the pipe
                // kill(parentPID,SIGUSR1);    //passing the signal to the parent
                write(fd[1],"user is Inactive!", 18);
                kill(parentPID,SIGUSR1);    //passing the signal to the parent
                close(fd[1]);
            }
            sigFlagChild = 0;
           }
        return 0;
    }
    else{
        close(fd[1]); //reading end of the pipe
        
        signal(SIGUSR1,signalhandlerParent);  //redirecting the signal from the parent.

        //loop through to countinously read the data in from the stdin
        while(true){
            //  scanf("%s",activity); //get the activity value from the user
                fgets(activity,sizeof(activity),stdin);
                activity[strcspn(activity, "\n")] = 0;  //compares the string and sets the value of that string to the null value

                if(strcmp(activity,"quit")==0){ //quit signal to terminate the program
                    kill(*childPID,SIGTERM);
                    break;
                }

                 kill(*childPID, SIGUSR1);
                 cout<<"!"<<activity<<"!"<<endl;
              sigFlagParent = 0;
        }
        wait(0);
    }
    close(fd[0]);



    return 0;

}
