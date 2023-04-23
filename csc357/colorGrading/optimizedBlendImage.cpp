#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>

using namespace std;

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

//creating a struct for each pixel
struct colorVal{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};
char grading(unsigned char value, float factor){


    value = value*factor;

    return value;
}


int main(){

    // if(argc !=4){
    //     printf("%s \n", "not the right number of arguments, try again");
    //     return 0;
    // }

    FILE *imageFile = fopen("wolf.bmp", "rb");   //open the file to read the content

    //checking weather the file is open or not
    if(imageFile ==NULL){
        cout<<"Something went wrong! File is not opened"<<endl;
    }
    else{
        cout<<"Opening the file to retrive the contents"<<endl<<endl;
    }

    tagBITMAPFILEHEADER readHeader;
    fread(&readHeader.bfType,sizeof(readHeader.bfType),1,imageFile); //have to sequentially read the file content
    fread(&readHeader.bfSize,sizeof(readHeader.bfSize),1,imageFile); 
    fread(&readHeader.bfReserved1,sizeof(readHeader.bfReserved1),1,imageFile); 
    fread(&readHeader.bfReserved2,sizeof(readHeader.bfReserved2),1,imageFile); 
    fread(&readHeader.bfOffBits,sizeof(readHeader.bfOffBits),1,imageFile); 

    //sequentially reading the file content from image header

    tagBITMAPINFOHEADER infoheader;
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

     colorVal * val = NULL;


    //adding padding
    LONG widthBytes,padding,real_width;

    widthBytes = (infoheader.biWidth*3);
    padding = 4-(widthBytes%4);
    real_width = widthBytes+padding;
    //adding padding


    //assigning block of memory on heap and using val to keep track of prgm brk
    val = (colorVal *)mmap(NULL,(real_width*infoheader.biHeight*sizeof(colorVal)),PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0);
    

    fseek(imageFile,readHeader.bfOffBits,SEEK_SET); //offset it to the pixel data

    // colorVal some;


     //read the content of the file in dynamic array
    for(int t =0;t<(real_width*infoheader.biHeight);t++){
        if(t%(infoheader.biWidth*3) ==0){
            val[t] = 0;
        }
        else{
            fread(&(val[t]),sizeof(val[t]),1,imageFile);
        // fread(&((val+t)->blue),sizeof((val+t)->blue),1,imageFile);
        // fread(&((val+t+1)->green),sizeof((val+t+1)->green),1,imageFile);
        // fread(&((val+t+2)->red),sizeof((val+t+2)->red),1,imageFile);
        }
        // val[i] = grading(some,1,1.5,1);
    }

    clock_t a,b = clock();
    int pid;
    cout<<"Starting time: "<<a<<endl;

    LONG halfHeight = (infoheader.biHeight*real_width)/2;
    pid = fork();
    if(pid==0){  //child process
        // performing the colorgrading
        // cout<<"we are in child process!"<<endl;
    for(int i =0;i<((halfHeight));i=i+3){
        (val+i)->blue = grading((val+i)->blue,0);
        (val+i+1)->green = grading((val+i+1)->green,1);
        (val+i+2)->red = grading((val+i+2)->red,0);
        }
        return 0;
    }
    else if(pid>0){   //parent process
    // cout<<"we are in the parent process"<<endl;
        for(int j =(halfHeight);j<(real_width*infoheader.biHeight);j=j+3){
        (val+j)->blue = grading((val+j)->blue,1);
        (val+j+1)->green = grading((val+j+1)->green,0);
        (val+j+2)->red = grading((val+j+2)->red,0);
        }
    }
    else{
        cout<<"Error in forking"<<endl;
        exit(1);
    }

    wait(0);

    b = clock();
    cout<<"Ending time: "<<b<<endl;
    cout<<"Total Run time :"<<(b-a)<<endl;



    fclose(imageFile);  //close the file after reading it


        FILE *aFile = fopen("lionLogic.bmp", "wb");   //open the file to write the content

    //checking weather the file is open or not
    if(aFile ==NULL){
        cout<<"Something went wrong! File is not opened"<<endl;
    }
    else{
        cout<<"Opening the file to write contents!"<<endl<<endl;
    


    fwrite(&readHeader.bfType,sizeof(readHeader.bfType),1,aFile); //have to sequentially write the file content
    fwrite(&readHeader.bfSize,sizeof(readHeader.bfSize),1,aFile); 
    fwrite(&readHeader.bfReserved1,sizeof(readHeader.bfReserved1),1,aFile); 
    fwrite(&readHeader.bfReserved2,sizeof(readHeader.bfReserved2),1,aFile); 
    fwrite(&readHeader.bfOffBits,sizeof(readHeader.bfOffBits),1,aFile); 

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


    fseek(aFile,readHeader.bfOffBits,SEEK_SET);

    for(int p =0;p<(real_width*infoheader.biHeight);p=p+3){

        fwrite(&((val+p)->blue),sizeof((val+p)->blue),1,aFile);
        fwrite(&((val+p+1)->green),sizeof((val+p+1)->green),1,aFile);
        fwrite(&((val+p+2)->red),sizeof((val+p+2)->red),1,aFile);
    }

    //closing the file
    fclose(aFile);

    //deallocating the memory 
    munmap(val,(real_width*infoheader.biHeight*sizeof(colorVal)));

    }
    return 0;

}


// int main(){
//     int pid;

//     pid = fork();

//     if(pid==0){
//         cout<<"child"<<endl;
//     }
//     else{
//         cout<<"parent"<<endl;
//         wait(0);
//     }
// }