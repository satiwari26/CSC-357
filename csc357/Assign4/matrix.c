#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#define MATRIX_DIMENSION_XY 10


//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M,int x,int y,float f)
    {
    M[x + y*MATRIX_DIMENSION_XY] = f;
    }
//************************************************************************************************************************
// lets see it both are the same
int quadratic_matrix_compare(float *A,float *B)
{
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
       if(A[a +b * MATRIX_DIMENSION_XY]!=B[a +b * MATRIX_DIMENSION_XY]) 
        return 0;
   
return 1;
}
//************************************************************************************************************************
//print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    {
    printf("\n");
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        printf("%.2f,",C[a + b* MATRIX_DIMENSION_XY]);
    }
printf("\n");
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A,float *B,float *C)
{
	//nullify the result matrix first
for(int a = 0;a<MATRIX_DIMENSION_XY;a++)
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++)
        C[a + b*MATRIX_DIMENSION_XY] = 0.0;
//multiply
for(int a = 0;a<MATRIX_DIMENSION_XY;a++) // over all cols a
    for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
        for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
            {
                C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
            }
}

void quadratic_matrix_multiplication_parallel(int par_id, int par_count,float *A, float *B, float *C){
        int row_distr = MATRIX_DIMENSION_XY/par_count;  //find the distribution to divide the rows between all childs equally
        int start_pos = row_distr*par_id;   //decide the starting row 
        int ending_pos = row_distr*par_id + row_distr; //decide the ending position of the row

        if(par_id==par_count-1){    //in case if rows are not perfectly divisible by the childs
            ending_pos = MATRIX_DIMENSION_XY;
        }

        for(int a = start_pos;a<ending_pos;a++) // over all cols a
            for(int b = 0;b<MATRIX_DIMENSION_XY;b++) // over all rows b
                for(int c = 0;c<MATRIX_DIMENSION_XY;c++) // over all rows/cols left
                    {
                        C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
                    }


        // for(int i=start_pos;i<ending_pos;i++){  //for testing only need to comeback to this later
        //     for(int j=0;j<MATRIX_DIMENSION_XY;j++){
        //         C[i + j*MATRIX_DIMENSION_XY] = par_id;
        //     }
        // }
}



//************************************************************************************************************************
void synch(int par_id,int par_count,int *ready, int gatherVal)
{

    // bool turnFlag = false;
    ready[par_id] = ready[par_id]+1;  //set the current process

    for(int i=0;i<par_count;i++){   //to gather all process
        while(ready[i]<gatherVal){
            //do nothing
        }
    }

}
//************************************************************************************************************************
int main(int argc, char *argv[])
{
    fflush(stdout);
    int par_id = 0; // the parallel ID of this process
    int par_count = 1; // the amount of processes
    float *A,*B,*C; //matrices A,B and C
    int *ready; //needed for synch
    float *averageTime;
    clock_t cb,ca;

    ca = clock();
    srand(time(NULL));  //generating the rand function seed

if(argc!=3){printf("no shared\n");}
else
    {
    par_id= atoi(argv[1]);
    par_count= atoi(argv[2]);
   // strcpy(shared_mem_matrix,argv[3]);
    }
if(par_count==1){printf("only one process\n");}

int fd[5];
if(par_id==0)
    {
        //if first child process
        fd[0] = shm_open("matrixA",O_RDWR|O_CREAT,0777);
        fd[1] = shm_open("matrixB",O_RDWR|O_CREAT,0777);
        fd[2] = shm_open("matrixC",O_RDWR|O_CREAT,0777);
        fd[3] = shm_open("syncObject",O_RDWR|O_CREAT,0777);
        fd[4] = shm_open("averageTime",O_RDWR|O_CREAT,0777);

        ftruncate(fd[4],sizeof(float));
        averageTime = (float*)mmap(NULL,sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[4],0);   //to calculate the average time of all the processes

        ftruncate(fd[3],par_count*sizeof(int));
        ready = (int *)mmap(NULL,par_count*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[3],0);   //to keep track of all the processes status
        
        ftruncate(fd[0],100*sizeof(float)); //allocate 10*10 size of the memory for all the matrixes
        ftruncate(fd[1],100*sizeof(float));
        ftruncate(fd[2],100*sizeof(float));
        A = (float*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[0],0); //allocate memory for sharing
        B = (float*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[1],0);
        C = (float*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[2],0);

        for(int i=0;i<par_count;i++){
        ready[i] = 0;   //initialize ready to 0 for all the processes
        }
    }
else
    {
        sleep(2); //needed for initalizing synch

        //if other child process will open it only for the reading
        fd[0] = shm_open("matrixA",O_RDWR,0777);
        fd[1] = shm_open("matrixB",O_RDWR,0777);
        fd[2] = shm_open("matrixC",O_RDWR,0777);
        fd[3] = shm_open("syncObject",O_RDWR,0777);
        fd[4] = shm_open("averageTime",O_RDWR,0777);

        averageTime = (float*)mmap(NULL,sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[4],0);   //to calculate the average time of all the processes

        ready = (int *)mmap(NULL,par_count*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[3],0);   //to keep track of all the processes status
        
        A = (float*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[0],0); //allocate memory for sharing
        B = (float*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[1],0);
        C = (float*)mmap(NULL,100*sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,fd[2],0);
    }

// printf("%d Before \n",par_id);
synch(par_id,par_count,ready,1);
// printf("%d After \n",par_id);

if(par_id==0)
    {

        for(int i=0;i<MATRIX_DIMENSION_XY;i++){
            for(int j=0;j<MATRIX_DIMENSION_XY;j++){
                set_matrix_elem(A,j,i,rand()%200);
                set_matrix_elem(B,j,i,rand()%200);
            }
        }
    }
// printf("%d Before \n",par_id);
synch(par_id,par_count,ready,2);
// printf("%d After \n",par_id);

    quadratic_matrix_multiplication_parallel( par_id, par_count,A, B, C); //performs multiplication based on the child id

// printf("%d Before \n",par_id);	
synch(par_id,par_count,ready,3);
// printf("%d After \n",par_id);

if(par_id==0){

    quadratic_matrix_print(C);
    // quadratic_matrix_print(A);
    // quadratic_matrix_print(B);
}

// printf("%d Before \n",par_id);
synch(par_id, par_count, ready,4);
// printf("%d After \n",par_id);
cb = clock();
*averageTime = (*averageTime + ((double)(cb-ca)))/par_count;
double average = *averageTime/ CLOCKS_PER_SEC *1000000;

synch(par_id, par_count, ready,5);
if(par_id==0){
    printf("\n");
    printf("Average time taken by all the processes(in micro Sec): %f\n",average);
    printf("\n");
}
synch(par_id, par_count, ready,6);

//lets test the result:
float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
quadratic_matrix_multiplication(A, B, M);
if (quadratic_matrix_compare(C, M))
	printf("full points!\n");
else
	printf("buuug!\n");


close(fd[0]);
close(fd[1]);
close(fd[2]);
close(fd[3]);
shm_unlink("matrixA");
shm_unlink("matrixB");
shm_unlink("matrixC");
shm_unlink("synchobject");
close(fd[4]);
shm_unlink("averageTime");

fflush(stdout);
return 0;    
}