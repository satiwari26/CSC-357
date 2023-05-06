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

    int fd = shm_open("cubeMem",O_RDWR|O_CREAT,0777);
    ftruncate(fd,100*sizeof(int));

    int *p = (int*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    p[0] = 100;

    sleep(10);

    close(fd);
    shm_unlink("cubeMem");
    munmap(p,100*sizeof(int));

    return 0;
}