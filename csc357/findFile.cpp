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

//char *fileName,char *startDir, char * result,

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

// char *tempStore = new char[5000]; //global temp to access  it again

// char *tempStartDir = new char[5000]; //temp dir for recursive calls

bool findFile( int toBeSearchedIn,char * startDir, char * fileName,char *result){
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
            cout<<"something went wrong!"<<endl;
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
                dirTell = dirType(statinfo);
            
                cout<<dirTell<<endl;
                if(strcmp(fileName,entry->d_name)==0){
                    result[0] = '0';
                    result = strcat(startDir,"/");
                    result = strcat(result,fileName);
                    cout<<result<<endl;
                    fileFound = 1;
                    free(tempStore);
                    closedir(dir);
                    return fileFound;
                    // break;
                }
            }
        }
        if(fileFound ==0){
            cout<<"No File with this name found."<<endl;
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
            cout<<"something went wrong!"<<endl;
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

                        fileFound = findFile(1,tempStartDir,fileName,result);  //recurisvely calling the function to look inside the dir until the file is found
                    }
                }

                if(fileFound == 1){ //break out of the recursion
                    free(tempStartDir);
                    free(tempStore);
                    closedir(dir); //close the current open directory
                    return fileFound;
                }
            
                // cout<<dirTell<<endl;
                if(strcmp(fileName,entry->d_name)==0){
                    strcpy(tempStartDir,startDir);
                    result[0] = '0';
                    result = strcat(tempStartDir,"/");
                    result = strcat(result,fileName);
                    cout<<result<<endl;
                    fileFound = 1;
                    free(tempStore);
                    free(tempStartDir);
                    closedir(dir); //close the current open directory
                    return fileFound;
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


int main(){

    char result[1000];
    char fileName[1000] = "singleLinkedList.cpp";
    char startDir[1000] = "/";

    findFile(1,startDir,fileName,result);



    return 0;
}