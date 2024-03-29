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
colorVal grading(colorVal value, float R_factor,float G_factor,float B_factor){

    // value.red = (255*value.red*R_factor)/(value.green+value.blue+value.red+255);
    // value.green = (255*value.green*G_factor)/(value.green+value.blue+value.red+255);
    // value.blue = (255*value.blue*B_factor)/(value.green+value.blue+value.red+255);

    value.red = value.red*R_factor;
    value.green = value.green*G_factor;
    value.blue = value.blue*B_factor;

    return value;
}


int main(int argc, char **argv){

    if(argc !=6){
        printf("%s \n", "not the right number of arguments, try again");
        return 0;
    }

    FILE *imageFile = fopen(argv[1], "rb");   //open the file to read the content

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

    widthBytes = (infoheader.biWidth);
    padding = 4-(widthBytes%4);
    real_width = widthBytes+padding;
    //adding padding


    //assigning block of memory on heap and using val to keep track of prgm brk
    val = (colorVal *)mmap(NULL,(real_width*infoheader.biHeight*sizeof(colorVal)),PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,-1,0);


    fseek(imageFile,readHeader.bfOffBits,SEEK_SET); //offset it to the pixel data

    // colorVal some;

     //read the content of the file in dynamic array
    for(int t =0;t<(real_width*infoheader.biHeight);t++){
        fread(&val[t],sizeof(val[t]),1,imageFile);
        // val[i] = grading(some,1,1.5,1);
    }

    clock_t a = clock();
    float StartSec = (float)a/CLOCKS_PER_SEC;
    int pid;
    cout<<"Starting time: "<<StartSec<<endl;

    LONG halfHeight = (infoheader.biHeight)/2;
    pid = fork();
    if(pid==0){  //child process
        // performing the colorgrading
        // cout<<"we are in child process!"<<endl;
    for(int i =0;i<(real_width*halfHeight);i++){
        val[i] = grading(val[i],atof(argv[2]),atof(argv[3]),atof(argv[4]));
        }
        return 0;
    }
    else if(pid>0){   //parent process
    // cout<<"we are in the parent process"<<endl;
        for(int j =(real_width*halfHeight);j<(real_width*infoheader.biHeight);j++){
        val[j] = grading(val[j],atof(argv[2]),atof(argv[3]),atof(argv[4]));
        }
    }
    else{
        cout<<"Error in forking"<<endl;
        exit(1);
    }

    wait(0);

    a = clock();
    float EndSec= (float)a/CLOCKS_PER_SEC;
    cout<<"Ending time: "<<EndSec<<endl;
    cout<<"Total Run time :"<<(EndSec-StartSec)<<endl;



    fclose(imageFile);  //close the file after reading it


        FILE *aFile = fopen(argv[5], "wb");   //open the file to write the content

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

    for(int p =0;p<(real_width*infoheader.biHeight);p++){
       fwrite(&val[p],sizeof(val[p]),1,aFile);
    }

    //closing the file
    fclose(aFile);

    //deallocating the memory 
    munmap(val,(real_width*infoheader.biHeight*sizeof(colorVal)));

    }
    return 0;

}
