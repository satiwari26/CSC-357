#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<signal.h>
#include <iostream>
#include<time.h>
#include <sys/wait.h>
using namespace std;

int fd[2];
int sigFlagChild =0;
int sigFlagParent =0;
//creating a signal handler function
void signalhandlerParent(int sig){
    dup2(fd[0], STDIN_FILENO);  //overwriting the stdin with fd[0]
    sigFlagParent = 1;
}

void signalhandlerChild(int signal){
        // dup2(fd[0], STDIN_FILENO);  //overwriting the stdin with fd[0]
    sigFlagChild = 1;
}

int main(){
    //creating a pipe function to communicate b/w child and parent
    pipe(fd);
    int parentPID = getpid();
    char text[1000];
    char activity[30];

    //forking into parent and child processes
    if(fork()==0){
           while(true){
            time_t sTime = time(NULL); //start time
            int eTime = 0;

            while(eTime < 2){
                signal(SIGUSR1,signalhandlerChild);
                eTime = time(NULL)-sTime;
            }
            if(sigFlagChild !=1){
                close(fd[0]);   //writing end of the pipe
                kill(parentPID,SIGUSR1);    //passing the signal to the parent
                write(fd[1],"nothing", 21);
                close(fd[1]);
            }
            sigFlagChild = 0;
           }
        return 0;
    }
    else{
        close(fd[1]); //reading end of the pipe
        int save_stdin = dup(STDIN_FILENO);

        //loop through to countinously read the data in from the stdin
        while(true){
            signal(SIGUSR1,signalhandlerParent);  //redirecting the signal from the parent.
            if(sigFlagParent ==1){
            read(STDIN_FILENO, text,21);    //read the value in from the other end of the pipe
            cout<<text<<endl;
            dup2(save_stdin,STDIN_FILENO);
            }
            else{
            scanf("%s",text); //get the text value from the user
            }
            //signal(SIGUSR1,signalhandlerParent);  //redirecting the signal from the parent.

            // if(sigFlagParent ==1){
            // read(STDIN_FILENO, activity, 21);    //read the value in from the other end of the pipe
            // cout<<activity<<endl;
            // dup2(save_stdin,STDIN_FILENO);
            // }
            sigFlagParent =0;
        }
        wait(0);
    }
    close(fd[0]);



    return 0;

}
