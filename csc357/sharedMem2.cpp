#include <iostream>
#include <sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>

using namespace std;


int main(){

    int fd = shm_open("cubeMem",O_RDWR,0777);
    char *p = (char*)mmap(NULL,100*sizeof(char),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

     while(true){

        if(fd ==-1){
            cout<<"not assigned"<<endl;
            break;
        }
        else{
        printf("%s \n", p);
        //break;
        }

     }
    close(fd);
    shm_unlink("cubeMem");
    munmap(p,100*sizeof(char));

    return 0;
}