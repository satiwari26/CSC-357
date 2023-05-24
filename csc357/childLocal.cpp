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

bool * ChildCount = (bool *)mmap(NULL,10*sizeof(bool),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);  
//global child count to keep track of the childs that are in use

int fd[2]; //pipline file ends, for reading and writing the files

void performFork(int parentPID, int *childPID, int offsetVal){
    if(fork()==0){
        close(fd[0]);   //writing through the pipe from the child
        *(childPID + offsetVal) = getpid();

        write(fd[1],&offsetVal,sizeof(offsetVal));  //passing data through the pipe
        kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process
        close(fd[1]);
         sleep(5);
        ChildCount[offsetVal] = 0;  //set the offset value back to 0 (free to use again)
        exit(0); //child process terminates.
    }
}

void signalhandlerParent(int sig){
    int offsetVal = 0;
    close(fd[1]);
    read(fd[0],&offsetVal,sizeof(offsetVal));
    cout<<"Process interupted for the brief moment"<<endl;
    
    cout<<"child "<<offsetVal<<"is acitve test."<<endl;
}

int main(){
    pipe(fd);
    char activity[1000];
    activity[0] = '0';
    bool foundChildflag = 0;  //to keep track if program found any availaible child process to be used
    int offsetVal = 0;

    for(int i=0;i<10;i++){
        ChildCount[i] = 0;  //assign all the childs to 0 i.e not being use
    }
    int parentPID = getpid();

    //shared mem that will keep track of the 10 childs pid, sequentially marked with ChildCount[i]
    int *childPID = (int *)mmap(NULL,10*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    
    signal(SIGUSR1,signalhandlerParent);  //redirecting the signal from the parent.

    //loop through continously to take input from the user
    while(true){
        fgets(activity,sizeof(activity),stdin);
        activity[strcspn(activity, "\n")] = 0;

        if(strcmp(activity,"quit")==0){ //quit signal to terminate all the child Processes
            for(int i=0;i<10;i++){
                kill(childPID[i],SIGTERM);  //send each child signal to end
            }
            break;
            }

        if(activity[0] !='0'){
            for(int i=0;i<5;i++){
                if(ChildCount[i] != 1){

                    cout<<ChildCount[i]<<" i: "<<i<<endl;

                    ChildCount[i] = 1;
                    foundChildflag = 1;
                    offsetVal = i;
                    break;
                    
                }
            }
            if(foundChildflag==0){
                cout<<"no processes are free"<<endl;
            }
            else{
                 performFork(parentPID, childPID,offsetVal);
            }
        }
        activity[0] = '0';
        foundChildflag = 0;
    }

    return 0;
}