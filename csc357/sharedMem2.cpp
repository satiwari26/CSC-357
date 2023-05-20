#include <iostream>
#include <sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>
#include <string.h>

using namespace std;


int main(){

    int fd = shm_open("cubeMem",O_RDWR,0777);
    char *p = (char*)mmap(NULL,1000*sizeof(char),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

      while(true){

        if(fd ==-1){
            cout<<"not assigned! run the first program to fix this issue"<<endl;
            break;
        }
        else{
          sleep(1);
            if(strcmp(p,"quit")==0){
                break;
            }
            else{
                if(p[0]!='\0'){
                  cout<<p<<endl;
                  memset(p, '\0', sizeof(p));
                }
            }
        //break;
        }
      }
    close(fd);
    shm_unlink("cubeMem");
    munmap(p,1000*sizeof(char));

    return 0;
}