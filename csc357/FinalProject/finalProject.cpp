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

using namespace std;

//information of the image to be stored
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
struct tagBITMAPFILEHEADER
{
WORD bfType; //specifies the file type
DWORD bfSize; //specifies the size in bytes of the bitmap file
WORD bfReserved1; //reserved; must be 0
WORD bfReserved2; //reserved; must be 0
DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct tagBITMAPINFOHEADER
{
DWORD biSize; //specifies the number of bytes required by the struct
LONG biWidth; //specifies width in pixels
LONG biHeight; //species height in pixels
WORD biPlanes; //specifies the number of color planes, must be 1
WORD biBitCount; //specifies the number of bit per pixel
DWORD biCompression;//spcifies the type of compression
DWORD biSizeImage; //size of image in bytes
LONG biXPelsPerMeter; //number of pixels per meter in x axis
LONG biYPelsPerMeter; //number of pixels per meter in y axis
DWORD biClrUsed; //number of colors used by th ebitmap
DWORD biClrImportant; //number of colors that are important
};


//gloablly define the matrix_XY dimention based on the image width and height
 int MATRIX_DIMENSION_XY = 0;



//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M,int x,int y,float f)
    {
        f = (f/255.00); //normalize the value by dividing it by 255
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

        for(int a = start_pos;a<ending_pos;a++) 
            for(int b = 0;b<MATRIX_DIMENSION_XY*3;b = b+3) 
                for(int c = 0;c<MATRIX_DIMENSION_XY*3;c = c+3)
                    {
                        C[a + b*MATRIX_DIMENSION_XY] += A[(c/3) + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY];
                        C[a + (b+1)*MATRIX_DIMENSION_XY] += A[(c/3) + (b+1)*MATRIX_DIMENSION_XY] * B[a + (c+1)*MATRIX_DIMENSION_XY];  //offset by 1
                        C[a + (b+2)*MATRIX_DIMENSION_XY] += A[(c/3) + (b+2)*MATRIX_DIMENSION_XY] * B[a + (c+2)*MATRIX_DIMENSION_XY];   //offset by 2
                    }




        // for(int i=start_pos;i<ending_pos;i++){  //for testing only need to comeback to this later
        //     for(int j=0;j<MATRIX_DIMENSION_XY;j++){
        //         C[i + j*MATRIX_DIMENSION_XY] = par_id;
        //     }
        // }
}

FILE* readingFileContent(tagBITMAPFILEHEADER &readHeader,tagBITMAPINFOHEADER &infoheader, char *fileName){

    FILE *imageFile = fopen(fileName, "rb");   //open the file to read the content

    //checking weather the file is open or not
    if(imageFile ==NULL){
        printf("Something went wrong! File is not opened \n");
    }
    else{
        // printf("Opening the file to retrive the contents \n \n");
    }
    
    fread(&readHeader.bfType,sizeof(readHeader.bfType),1,imageFile); //have to sequentially read the file content
    fread(&readHeader.bfSize,sizeof(readHeader.bfSize),1,imageFile); 
    fread(&readHeader.bfReserved1,sizeof(readHeader.bfReserved1),1,imageFile); 
    fread(&readHeader.bfReserved2,sizeof(readHeader.bfReserved2),1,imageFile); 
    fread(&readHeader.bfOffBits,sizeof(readHeader.bfOffBits),1,imageFile); 

    //sequentially reading the file content from image header

    fread(&infoheader.biSize,sizeof(infoheader.biSize),1,imageFile);
    fread(&infoheader.biWidth,sizeof(infoheader.biWidth),1,imageFile);
    fread(&infoheader.biHeight,sizeof(infoheader.biHeight),1,imageFile);
    fread(&infoheader.biPlanes,sizeof(infoheader.biPlanes),1,imageFile);
    fread(&infoheader.biBitCount,sizeof(infoheader.biBitCount),1,imageFile);
    fread(&infoheader.biCompression,sizeof(infoheader.biCompression),1,imageFile);
    fread(&infoheader.biSizeImage,sizeof(infoheader.biSizeImage),1,imageFile);
    fread(&infoheader.biXPelsPerMeter,sizeof(infoheader.biXPelsPerMeter),1,imageFile);
    fread(&infoheader.biYPelsPerMeter,sizeof(infoheader.biYPelsPerMeter),1,imageFile);
    fread(&infoheader.biClrUsed,sizeof(infoheader.biClrUsed),1,imageFile);
    fread(&infoheader.biClrImportant,sizeof(infoheader.biClrImportant),1,imageFile);
    

    return imageFile;   //return the 
}

FILE * writingFileContent(tagBITMAPFILEHEADER &writeHeader,tagBITMAPINFOHEADER &infoheader){

    FILE *aFile = fopen("testRun.bmp", "wb");   //open the file to write the content

    //checking weather the file is open or not
    if(aFile ==NULL){
       printf("Something went wrong! File is not opened \n");
        return 0;
    }
    else{
        // printf("Opening the file to retrive the contents \n \n");
    }

        fwrite(&writeHeader.bfType,sizeof(writeHeader.bfType),1,aFile); //have to sequentially write the file content and would be same as larger file
        fwrite(&writeHeader.bfSize,sizeof(writeHeader.bfSize),1,aFile); 
        fwrite(&writeHeader.bfReserved1,sizeof(writeHeader.bfReserved1),1,aFile); 
        fwrite(&writeHeader.bfReserved2,sizeof(writeHeader.bfReserved2),1,aFile); 
        fwrite(&writeHeader.bfOffBits,sizeof(writeHeader.bfOffBits),1,aFile); 

        //sequentially writing the file content from image header

        fwrite(&infoheader.biSize,sizeof(infoheader.biSize),1,aFile);
        fwrite(&infoheader.biWidth,sizeof(infoheader.biWidth),1,aFile);
        fwrite(&infoheader.biHeight,sizeof(infoheader.biHeight),1,aFile);
        fwrite(&infoheader.biPlanes,sizeof(infoheader.biPlanes),1,aFile);
        fwrite(&infoheader.biBitCount,sizeof(infoheader.biBitCount),1,aFile);
        fwrite(&infoheader.biCompression,sizeof(infoheader.biCompression),1,aFile);
        fwrite(&infoheader.biSizeImage,sizeof(infoheader.biSizeImage),1,aFile);
        fwrite(&infoheader.biXPelsPerMeter,sizeof(infoheader.biXPelsPerMeter),1,aFile);
        fwrite(&infoheader.biYPelsPerMeter,sizeof(infoheader.biYPelsPerMeter),1,aFile);
        fwrite(&infoheader.biClrUsed,sizeof(infoheader.biClrUsed),1,aFile);
        fwrite(&infoheader.biClrImportant,sizeof(infoheader.biClrImportant),1,aFile);


    return aFile;


    //part of writing will come back to this later

    // unsigned char colorVal;
    //     imageFilef1 = writingFileContent(readHeaderf1,infoheaderf1);
    //     fseek(imageFilef1,readHeaderf1.bfOffBits,SEEK_SET);    //image 1 data in matrix A
    //         for(int i=0;i<MATRIX_DIMENSION_XY;i++){
    //             for(int j=0;j<MATRIX_DIMENSION_XY;j++){
    //                 colorVal = (char)B[i + j*MATRIX_DIMENSION_XY];
    //                 fwrite(&colorVal,sizeof(colorVal),1,imageFilef1);
    //             }
    //         }

    //         fclose(imageFilef1);
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
    float *averageTime; //time take by all the processes to complete the process
    clock_t cb,ca;
    char fileNamef1[] = "f0.bmp";
    char fileNamef2[] = "f2.bmp";
    FILE * imageFilef1 = NULL;
    FILE * imageFilef2 = NULL;

    ca = clock();   //starting the clock timer

    tagBITMAPFILEHEADER readHeaderf1;   //File header for the f1
    tagBITMAPINFOHEADER infoheaderf1;   //info header for the f1
    imageFilef1 = readingFileContent(readHeaderf1,infoheaderf1,fileNamef1);

    tagBITMAPFILEHEADER readHeaderf2;   //File header for the f1
    tagBITMAPINFOHEADER infoheaderf2;   //info header for the f1
    imageFilef2 = readingFileContent(readHeaderf2,infoheaderf2,fileNamef2);

    //adding padding
    LONG widthBytes=0,padding=0,real_width=0;

    widthBytes = (infoheaderf1.biWidth);
    if(widthBytes%4 !=0)
    {
    padding = 4-(widthBytes%4);
    }
    real_width = widthBytes+padding;

    MATRIX_DIMENSION_XY = real_width;  //define since the width and heigh are same


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
            
            ftruncate(fd[0],(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float)); //allocate (MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3) size of the memory for all the matrixes
            ftruncate(fd[1],(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float));
            ftruncate(fd[2],(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float));
            A = (float*)mmap(NULL,(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[0],0); //allocate memory for sharing
            B = (float*)mmap(NULL,(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[1],0);
            C = (float*)mmap(NULL,(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[2],0);

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
            
            A = (float*)mmap(NULL,(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[0],0); //allocate memory for sharing
            B = (float*)mmap(NULL,(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[1],0);
            C = (float*)mmap(NULL,(MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*3)*sizeof(float),PROT_READ|PROT_WRITE,MAP_SHARED,fd[2],0);
        }

    // printf("%d Before \n",par_id);
    synch(par_id,par_count,ready,1);
    // printf("%d After \n",par_id);

    if(par_id==0)
        {
            unsigned char colorVal; //to read the rgb value in bytes

             fseek(imageFilef1,readHeaderf1.bfOffBits,SEEK_SET);    //image 1 data in matrix A
            for(int i=0;i<MATRIX_DIMENSION_XY;i++){
                for(int j=0;j<infoheaderf1.biWidth*3;j =j+3){
                    fread(&colorVal,sizeof(colorVal),1,imageFilef1);   //read the individual bytes from the bmp file
                    set_matrix_elem(B,i,j,(float)colorVal); //populate the matrix with image blue data

                    fread(&colorVal,sizeof(colorVal),1,imageFilef1);   //read the individual bytes from the bmp file
                    set_matrix_elem(B,i,j+1,(float)colorVal); //populate the matrix with image green data

                    fread(&colorVal,sizeof(colorVal),1,imageFilef1);   //read the individual bytes from the bmp file
                    set_matrix_elem(B,i,j+2,(float)colorVal); //populate the matrix with image red data
                }
            }

            fclose(imageFilef1);    //close file 1 after reading


             fseek(imageFilef2,readHeaderf2.bfOffBits,SEEK_SET);    //image 2 data in matrix A
            for(int i=0;i<MATRIX_DIMENSION_XY;i++){
                for(int j=0;j<infoheaderf1.biWidth*3;j=j+3){
                    fread(&colorVal,sizeof(colorVal),1,imageFilef2);   //read the individual bytes from the bmp file
                    set_matrix_elem(A,i,j,(float)colorVal); //populate the matrix with image blue2 data

                    fread(&colorVal,sizeof(colorVal),1,imageFilef2);   //read the individual bytes from the bmp file
                    set_matrix_elem(A,i,j+1,(float)colorVal); //populate the matrix with image green2 data

                    fread(&colorVal,sizeof(colorVal),1,imageFilef2);   //read the individual bytes from the bmp file
                    set_matrix_elem(A,i,j+2,(float)colorVal); //populate the matrix with image red2 data
                }
            }

            fclose(imageFilef2);    //close file2 after reading
        }
    // printf("%d Before \n",par_id);
    synch(par_id,par_count,ready,2);
    // printf("%d After \n",par_id);

         quadratic_matrix_multiplication_parallel( par_id, par_count,A, B, C); //performs multiplication based on the child id

    // printf("%d Before \n",par_id);	
    synch(par_id,par_count,ready,3);
    // printf("%d After \n",par_id);

    if(par_id==0){
        for(int a = 0;a<MATRIX_DIMENSION_XY;a++){
            for(int b = 0;b<MATRIX_DIMENSION_XY*3;b = b+3){
                C[a + b*MATRIX_DIMENSION_XY] = C[a + b*MATRIX_DIMENSION_XY]*0.02;   //normaliz it so it doesn't exceed the char value
                C[a + b*MATRIX_DIMENSION_XY] = C[a + b*MATRIX_DIMENSION_XY]*255;

                C[a + (b+1)*MATRIX_DIMENSION_XY] = C[a + (b+1)*MATRIX_DIMENSION_XY]*0.02;   //normaliz it so it doesn't exceed the char value
                C[a + (b+1)*MATRIX_DIMENSION_XY] = C[a + (b+1)*MATRIX_DIMENSION_XY]*255;

                C[a + (b+2)*MATRIX_DIMENSION_XY] = C[a + (b+2)*MATRIX_DIMENSION_XY]*0.02;   //normaliz it so it doesn't exceed the char value
                C[a + (b+2)*MATRIX_DIMENSION_XY] = C[a + (b+2)*MATRIX_DIMENSION_XY]*255;
            }
        }

        unsigned char colorVal;
        imageFilef1 = writingFileContent(readHeaderf1,infoheaderf1);
        fseek(imageFilef1,readHeaderf1.bfOffBits,SEEK_SET);    //image 1 data in matrix A
            for(int i=0;i<MATRIX_DIMENSION_XY;i++){
                for(int j=0;j<infoheaderf1.biWidth*3;j=j+3){
                    colorVal = (char)C[i + j*MATRIX_DIMENSION_XY];  //blue value
                    fwrite(&colorVal,sizeof(colorVal),1,imageFilef1);

                    colorVal = (char)C[i + (j+1)*MATRIX_DIMENSION_XY];  //green value
                    fwrite(&colorVal,sizeof(colorVal),1,imageFilef1);

                    colorVal = (char)C[i + (j+2)*MATRIX_DIMENSION_XY];  //Red value
                    fwrite(&colorVal,sizeof(colorVal),1,imageFilef1);
                }
            }

            fclose(imageFilef1);
        
    }
    synch(par_id,par_count,ready,4);

    // if(par_id==0){

    //     // quadratic_matrix_print(C);
    //     // quadratic_matrix_print(A);
    //     // quadratic_matrix_print(B);
    // }

    // printf("%d Before \n",par_id);
    synch(par_id, par_count, ready,5);
    // printf("%d After \n",par_id);
    cb = clock();
    *averageTime = (*averageTime + ((double)(cb-ca)))/par_count;
    double average = *averageTime/ CLOCKS_PER_SEC *1000000;

    synch(par_id, par_count, ready,6);
    if(par_id==0){
        printf("\n");
        printf("Average time taken by all the processes(in micro Sec): %f\n",average);
        printf("\n");
    }
    synch(par_id, par_count, ready,7);

    // //lets test the result:
    // float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
    // quadratic_matrix_multiplication(A, B, M);
    // if (quadratic_matrix_compare(C, M))
    //     printf("full points!\n");
    // else
    //     printf("buuug!\n");


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