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

string dirType(struct stat statinfo){
    string type;
    switch (statinfo.st_mode & S_IFMT){
           case S_IFBLK:  type = "block device";            break;
           case S_IFCHR:  type = "character device";        break;
           case S_IFDIR:  type = "directory";               break;
           case S_IFIFO:  type = "FIFO/pipe";               break;
           case S_IFLNK:  type = "symlink";                 break;
           case S_IFREG:  type = "regular file";            break;
           case S_IFSOCK: type = "socket";                  break;
           default:       type = "unknown?";                break;
        }
    return type;
} 

void findFile( int toBeSearchedIn, char * fileName,char * result){
    DIR *dir;
    char workdir[1000];
    getcwd(workdir,1000);
    struct stat statinfo;
    struct dirent *entry;
    if(toBeSearchedIn ==0){
        dir = opendir(".");
        if(dir==NULL){
            cout<<"something went wrong!"<<endl;
            return;
        }
        for(entry = readdir(dir);entry !=NULL;entry = readdir(dir)){
            // cout<<entry->d_name<<endl;
            int resp = stat(entry->d_name,&statinfo);
            string dirTell = "";
            if(resp==0){
                dirTell = dirType(statinfo);
            
                // cout<<dirTell<<endl;
                if(strcmp(fileName,entry->d_name)==0){
                    result = strcat(workdir,"/");
                    result = strcat(result,fileName);
                    cout<<result<<endl;
                    // break;
                }
            }
        }
    }
}


int main(){

    char result[1000];
    char fileName[1000] = "SharedMem.cpp";

    findFile(0,fileName,result);



    return 0;
}