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

int save_stdin = dup(STDIN_FILENO); //save the stdin
char constDirPath[5000];   //const starting DIR path
char *combinedResult = new char[10*5000];  //a array that would hold multiple files path that are found

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


bool findFile( int toBeSearchedIn,char * startDir, char * fileName,char *result,int parentPID,int offsetVal){
    bool fileFound = false; //flag that specify if the file is found or not
    bool tempFileFound = false; //flag that specify if the recursive file is found or not
    DIR *dir;
    // char workdir[5000];
    // getcwd(workdir,5000);
    struct stat statinfo;
    struct dirent *entry;
    if(toBeSearchedIn ==0){ //search in the current dir
        dir = opendir(startDir);    //open dir access full string from the char ptr

        char *tempStore = new char[5000]; //global temp to access  it again

        if(dir==NULL){  
            // cout<<"something went wrong! Can't open this DIR"<<endl;
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
                    
                    // cout<<result<<endl;

                    write(fd[1],result,5000);  //passing data through the pipe
                    // kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process
                    

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
            // cout<<"something went wrong! Can't open this dir"<<endl;

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
                    if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0 && strcmp(entry->d_name,"mnt")!=0 && strcmp(entry->d_name,"proc")!=0 && strcmp(entry->d_name,"dev")!=0 && strcmp(entry->d_name,"sys")!=0 && strcmp(entry->d_name,"Application Data")!=0){  //rrxclude current and parent directory
                        strcpy(tempStartDir,startDir);
                        tempStartDir = strcat(tempStartDir,"/");
                        tempStartDir = strcat(tempStartDir,entry->d_name); //create new start dir path for recursive function.

                        tempFileFound = findFile(1,tempStartDir,fileName,result,parentPID, offsetVal);  //recurisvely calling the function to look inside the dir until the file is found
                        
                        if(tempFileFound ==1){  //only make changes to original fileFound if file is found.
                            fileFound = tempFileFound;
                        }
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
                    result = strcat(tempStartDir,"/");
                    result = strcat(result,fileName);
                    result = strcat(result,"\n");
                    strcat(((offsetVal*5000)+combinedResult),result);  //grouping multiple paths together
                    // cout<<result;
                    // write(fd[1],result,5000);  //passing data through the pipe
                    // kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process
                    fileFound = 1;  //set the file found to be 1

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
    if(strcmp(startDir,constDirPath)==0){   //so when the original dir func ends
        // cout<<((offsetVal*5000)+combinedResult);
        write(fd[1],((offsetVal*5000)+combinedResult),5000);  //passing data through the pipe
        *((offsetVal*5000)+combinedResult) = '\0';   //free the array again.
    }
    return fileFound;
}

//forking inside the function so that multiple times it can be called

void performFork(int parentPID, int *childPID, int offsetVal, int toBeSearchedIn,char * startDir, char * fileName){
    if(fork()==0){
        char result[5000];
        result[0] = '\0';
        close(fd[0]);   //writing through the pipe from the child
        // *(childPID + offsetVal) = getpid();
        bool fileFound =0;
        
        fileFound = findFile(toBeSearchedIn,startDir,fileName,result,parentPID, offsetVal);
        if(fileFound ==0){
            printf("\n");
            cout<<"No File with this name found."<<endl;

            printf("findFile$ ");
            fflush(stdout);

            fileFound = 0;
            ChildCount[offsetVal] = 0;  //set the offset value back to 0 (free to use again)
            exit(0); //terminate the child right away.
        }
        // else{
        //     write(fd[1],result,sizeof(result));  //passing data through the pipe
        //     kill(parentPID,SIGUSR1);    //passing the signal to parent to interrupt the process
        //     close(fd[1]);
        // }
        // sleep(10);

        *(childPID + offsetVal) = getpid(); //get the pid at the end of the process
        kill(parentPID,SIGUSR1); //pass the signal when it's almost done running

        sleep(2); //add sleep of 2 sec for the reading end to finish reading properly
        close(fd[1]);   //closing write only when the child process is about to end

        ChildCount[offsetVal] = 0;  //set the offset value back to 0 (free to use again)
        exit(0); //child process terminates.
        cout<<"checking if child is terminated or not"<<endl;
        
    }
    return;
}

int parse(char *userInput, char * arg, int c){  // to parse the input for the user
    int numberWords = 0;
    int startingPoint = 0;
    int endingPoint = 0;

    for(int i=0; i< strlen(userInput)+1;i++){
        if(userInput[i]==' ' || userInput[i]=='\0'){
            endingPoint = i;    //curr arguments end here
            if(numberWords == c){
                strncpy(arg,userInput+startingPoint,endingPoint-startingPoint);
                arg[endingPoint-startingPoint] = '\0';
                return 1;
            }
            startingPoint = endingPoint+1;
            numberWords++;
        }
    }

    return 0;
}


void signalhandlerParent(int sig){
    // char result1[5000];
    // result1[0] = '0';
    // close(fd[1]);   //no reading only writing
    // read(fd[0],result1,sizeof(result1));

    //  cout<<"Process interupted by Child"<<endl;

    // cout<<result1<<endl;
    ParentHandlerFlag = 1;

    // cout<<ParentHandlerFlag<<"inside the handler"<<endl;

    dup2(fd[0], STDIN_FILENO); //rewriting stdin with other end of the pipe
    
}


int main(){
    //file names and starting dir
    char fileName[5000];
    char argument[5000];    //to take the argument from the user

    bool proceedFlag = 0;   //to check if user input meets the requirement

    int temp =0; //store the temp return val from user
    char startDir[5000] = ".";
    int toBeSearchedIn = 1; //by default we are searching in the current directory only

    pipe(fd);
    fileName[0] = '\0';
    bool foundChildflag = 0;  //to keep track if program found any availaible child process to be used
    int offsetVal = 0;

    for(int i=0;i<10;i++){
        ChildCount[i] = 0;  //assign all the childs to 0 i.e not being use
    }
    int parentPID = getpid();   //pid of the parent

    int childStatus[10];    //to store the status of each child 

    //shared mem that will keep track of the 10 childs pid, sequentially marked with ChildCount[i]
    int *childPID = (int *)mmap(NULL,10*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    
    signal(SIGUSR1,signalhandlerParent);  //redirecting the signal from the parent.
    

    //loop through continously to take input from the user
    while(true){
        // fgets(fileName,sizeof(fileName),stdin);
        printf("findFile$ ");
        fflush(stdout); 
        read(STDIN_FILENO, fileName, sizeof(fileName));

        if(ParentHandlerFlag ==0){  //if child sends the data don't change '\n'
            fileName[strcspn(fileName, "\n")] = 0;

            parse(fileName,argument,0); //call parse function to check if first arugument is quit

            if(strcmp(argument,"quit")==0){ //quit signal to terminate all the child Processes
            for(int i=0;i<10;i++){
                kill(childPID[i],SIGTERM);  //send each child signal to end
            }
            break;
            }
            else{
                int parseChecker = 0;   //to check if user input meets the min number of inputs
                argument[0] = '\0';
                for(int i=0;i<4;i++){
                    temp = parse(fileName,argument,i);
                    parseChecker = parseChecker+temp;
                }
                if(parseChecker>3){
                    printf("\n");
                    cout<<"not the right number of args. Please enter right number of arguments!"<<endl;
                }
                else if(parseChecker<2){
                    printf("\n");
                    cout<<"not the right number of args. Please enter right number of arguments!"<<endl;
                }
                else{
                    parse(fileName,argument,0);
                     if(strcmp(argument,"find")==0){
                        proceedFlag = 1;
                     }
                     else{
                        printf("\n");
                        cout<<"not the right input"<<endl;
                     }
                }
            }
        }



        for(int i=0;i<10;i++){  //wait for each child process non-blocking
            waitpid(childPID[i],&childStatus[i],WNOHANG);
        }

        // cout<<ParentHandlerFlag<<"inside the main"<<endl;

        if(ParentHandlerFlag ==1){
            dup2(save_stdin,STDIN_FILENO); //re-write the stdin again

            // cout<<"signal send by child"<<endl;
            printf("\n");
            printf("%s\n", fileName);
            // cout<<fileName<<endl; //from stdin (result is now stored in fileName)

            fileName[0] = '\0';
            argument[0] = '\0';
            ParentHandlerFlag = 0;  //set flag to false
        }

        if(proceedFlag ==1){    //input validation check
            argument[0] = '\0';
            parse(fileName,argument,2);
            if(argument[0] == '\0'){
                strcpy(startDir,".");
                strcpy(constDirPath,startDir);  //save the const starting dir path
                toBeSearchedIn = 0;
            }
            else if(strcmp(argument,"s")==0 || strcmp(argument,"-s")==0){
                strcpy(startDir,".");
                strcpy(constDirPath,startDir);  //save the const starting dir path
                toBeSearchedIn = 1;
            }
            else if(strcmp(argument,"f")==0 || strcmp(argument,"-f")==0){
                strcpy(startDir,"/");
                strcpy(constDirPath,startDir);  //save the const starting dir path
                toBeSearchedIn = 1;
            }

                for(int i=0;i<10;i++){
                    if(ChildCount[i] != 1){

                        // cout<<ChildCount[i]<<" i: "<<i<<endl;

                        ChildCount[i] = 1;
                        foundChildflag = 1;
                        offsetVal = i;
                        break;
                    }
                }
                if(foundChildflag==0){
                    printf("\n");
                    cout<<"no processes are free"<<endl;
                }
                else{
                    argument[0] = '\0';
                    parse(fileName,argument,1); //get the file that we want to search
                    performFork(parentPID, childPID,offsetVal,toBeSearchedIn,startDir,argument);
                }
        }
        memset(fileName, 0, sizeof(fileName));
        memset(argument, 0, sizeof(argument));
        proceedFlag = 0;
        foundChildflag = 0;
    }
    //close(fd[0]);   //close reading after the whole process is ended
    free(combinedResult);
    return 0;
}


