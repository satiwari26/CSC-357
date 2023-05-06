#include <iostream>
#include <sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>
#include<string.h>

using namespace std;


int main(){

    int fd = shm_open("cubeMem",O_RDWR|O_CREAT,0777);
    ftruncate(fd,1000*sizeof(int));

    char *p = (char*)mmap(NULL,1000*sizeof(char),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    
    while(strcmp(p,"quit")!=0){
   //strcpy(p,"my name is saumitra");
    //scanf("%s",p);
    fgets(p,1000*sizeof(p),stdin);
    p[strcspn(p, "\n")] = 0;
     //sleep(1);
    }

    close(fd);
    shm_unlink("cubeMem");
    munmap(p,1000*sizeof(char));

    return 0;
}