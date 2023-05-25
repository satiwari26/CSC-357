#include <dirent.h>
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
#include <sys/stat.h>

using namespace std;

bool * ChildCount = (bool *)mmap(NULL,10*sizeof(bool),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);  
//global child count to keep track of the childs that are in use

int fd[2]; //pipline file ends, for reading and writing the files
int ParentHandlerFlag = 0; //flag to check if the signal was received successfully or not


char *dirType(struct stat statinfo){
    char *type = new char[100];
    switch (statinfo.st_mode & S_IFMT){
           case S_IFBLK:  strcpy(type,"block device");            break;
           case S_IFCHR:    strcpy(type,"character device");        break;
           case S_IFDIR:    strcpy(type,"directory");              break;
           case S_IFIFO:    strcpy(type,"FIFO/pipe");               break;
           case S_IFLNK:    strcpy(type,"symlink");                break;
           case S_IFREG:    strcpy(type,"regular file");            break;
           case S_IFSOCK:   strcpy(type,"socket");                 break;
           default:         strcpy(type,"unknown?");               break;
        }
    return type;
} 


bool findFile( int toBeSearchedIn,char * startDir, char * fileName,char *result,int parentPID){
    bool fileFound = false; //flag that specify if the file is found or not
    DIR *dir;
    // char workdir[1000];
    // getcwd(workdir,1000);
    struct stat statinfo;
    struct dirent *entry;
    if(toBeSearchedIn ==0){ //search in the current dir
        dir = opendir(startDir);    //open dir access full string from the char ptr

        char *tempStore = new char[5000]; //global temp to access  it again

        if(dir==NULL){  
            cout<<"something went wrong! Can't open this DIR"<<endl;
            closedir(dir); //close the current open directory 
            free(tempStore);
            return fileFound;
        }
        for(entry = readdir(dir);entry !=NULL;entry = readdir(dir)){
            // cout<<entry->d_name<<endl;
            strcpy(tempStore,startDir);
            tempStore = strcat(tempStore,"/");

            tempStore = strcat(tempStore,entry->d_name);
            int resp = stat(tempStore,&statinfo);
            char *dirTell;
            if(resp==0){
                // dirTell = dirType(statinfo);
            
                // cout<<dirTell<<endl;
                if(strcmp(fileName,entry->d_name)==0){
                    result[0] = '0';
                    result = strcat(startDir,"/");
                    result = strcat(result,fileName);
                    cout<<result<<endl;

                    write(fd[1],result,1000);  //passing data through the pipe
                    kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process
                    

                    // cout<<result<<endl;
                    fileFound = 1;
                    free(tempStore);
                    closedir(dir);
                    return fileFound;
                    // break;
                }
            }
        }
        if(fileFound ==0){
            // cout<<"No File with this name found."<<endl;
            fileFound = 0;
            closedir(dir); //close the current open directory 
            free(tempStore);
            return fileFound;
        }
    }
    else if(toBeSearchedIn ==1){    //have to search in the current dir and my sub dir

        dir = opendir(startDir);    //open dir access full string from the char ptr

        char *tempStore = new char[5000]; //global temp to access  it again
        char *tempStartDir = new char[5000]; //temp dir for recursive calls

        if(dir==NULL){  
            cout<<"something went wrong! Can't open this dir"<<endl;

            free(tempStartDir);
            free(tempStore);
            closedir(dir); //close the current open directory
            return fileFound;
        }
        for(entry = readdir(dir);entry !=NULL;entry = readdir(dir)){

            strcpy(tempStore,startDir);
            tempStore = strcat(tempStore,"/");
            tempStore = strcat(tempStore,entry->d_name);

            int resp = stat(tempStore,&statinfo);   //stat needs full path of the file that we are looking for
            char *dirTell;
            if(resp==0){
                dirTell = dirType(statinfo);
                if(strcmp(dirTell,"directory")==0){
                    if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){  //rrxclude current and parent directory
                        strcpy(tempStartDir,startDir);
                        tempStartDir = strcat(tempStartDir,"/");
                        tempStartDir = strcat(tempStartDir,entry->d_name); //create new start dir path for recursive function.

                        fileFound = findFile(1,tempStartDir,fileName,result,parentPID);  //recurisvely calling the function to look inside the dir until the file is found
                    }
                }

                // if(fileFound == 1){ //break out of the recursion
                //     free(tempStartDir);
                //     free(tempStore);
                //     closedir(dir); //close the current open directory
                //     return fileFound;
                // }
            
                // cout<<dirTell<<endl;
                if(strcmp(fileName,entry->d_name)==0){
                    strcpy(tempStartDir,startDir);
                    result[0] = '0';
                    result = strcat(tempStartDir,"/");
                    result = strcat(result,fileName);

                    write(fd[1],result,1000);  //passing data through the pipe
                    kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process


                    // cout<<result<<endl;
                    // fileFound = 1;
                    // free(tempStore);
                    // free(tempStartDir);
                    // closedir(dir); //close the current open directory
                    // return fileFound;
                    // break;
                }
            }
        }
        closedir(dir); //close the current open directory
        free(tempStore);
        free(tempStartDir);
    }
    return fileFound;
}

//forking inside the function so that multiple times it can be called

void performFork(int parentPID, int *childPID, int offsetVal, int toBeSearchedIn,char * startDir, char * fileName,char *result){
    if(fork()==0){
        close(fd[0]);   //writing through the pipe from the child
        *(childPID + offsetVal) = getpid();
        bool fileFound =0;
        
        fileFound = findFile(toBeSearchedIn,startDir,fileName,result,parentPID);
        if(fileFound ==0){
            cout<<"No File with this name found."<<endl;
            fileFound = 0;
        }
        // else{
        //     write(fd[1],result,sizeof(result));  //passing data through the pipe
        //     kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process
        //     close(fd[1]);
        // }
         sleep(10);
        close(fd[1]);   //closing write only when the child process is about to end
        ChildCount[offsetVal] = 0;  //set the offset value back to 0 (free to use again)
        exit(0); //child process terminates.
    }
}


void signalhandlerParent(int sig){
    // char result1[1000];
    // result1[0] = '0';
    // close(fd[1]);   //no reading only writing
    // read(fd[0],result1,sizeof(result1));
     cout<<"Process interupted by Child"<<endl;
    // cout<<result1<<endl;
    ParentHandlerFlag = 1;
    cout<<ParentHandlerFlag<<"inside the handler"<<endl;
    
}


int main(){
    //file names and starting dir
    char result[1000];
    char fileName[1000];
    char startDir[5000] = "/home/chero/csc357";
    int toBeSearchedIn = 0; //by default we are searching in the current directory only

    pipe(fd);
    fileName[0] = '0';
    bool foundChildflag = 0;  //to keep track if program found any availaible child process to be used
    int offsetVal = 0;

    for(int i=0;i<10;i++){
        ChildCount[i] = 0;  //assign all the childs to 0 i.e not being use
    }
    int parentPID = getpid();   //pid of the parent

    //shared mem that will keep track of the 10 childs pid, sequentially marked with ChildCount[i]
    int *childPID = (int *)mmap(NULL,10*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    
    signal(SIGUSR1,signalhandlerParent);  //redirecting the signal from the parent.

    //loop through continously to take input from the user
    while(true){
        fgets(fileName,sizeof(fileName),stdin);
        fileName[strcspn(fileName, "\n")] = 0;

        cout<<ParentHandlerFlag<<"inside the main"<<endl;   //not sync with the handler
        if(ParentHandlerFlag ==1){
            cout<<"signal send by child"<<endl;
            ParentHandlerFlag = 0;  //set flag to false
        }

        if(strcmp(fileName,"quit")==0){ //quit signal to terminate all the child Processes
            for(int i=0;i<10;i++){
                kill(childPID[i],SIGTERM);  //send each child signal to end
            }
            break;
            }

        if(fileName[0] !='0'){
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
                 performFork(parentPID, childPID,offsetVal,toBeSearchedIn,startDir,fileName,result);
            }
        }
        fileName[0] = '0';
        foundChildflag = 0;
    }
    //close(fd[0]);   //close reading after the whole process is ended

    return 0;
}


