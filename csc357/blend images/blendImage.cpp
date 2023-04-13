#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

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

unsigned char power(unsigned char power,float scaling){
    if(scaling>1){
        return(pow(power,scaling)/255);
    }
    else if(scaling<1){
        return(pow(power,scaling));
    }
    else{
        return(power);
    }
}


int main(){

    FILE *imageFile = fopen("lion.bmp", "rb");   //open the file to read the content

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



  


    //unsigned char val[(infoheader.biHeight*3)][(infoheader.biWidth*3)];


    //unsigned char val[(infoheader.biHeight*3*infoheader.biWidth*3)];

    
    //unsigned ptr to point to the current program break on the heap
    unsigned char * val, *temp;

    //assigning block of memory on heap and using val to keep track of prgm brk
    val = (unsigned char *)sbrk((infoheader.biHeight*3*infoheader.biWidth*3));

    //current position of the prgm brk
    temp = (unsigned char *)sbrk(0);
    

    fseek(imageFile,readHeader.bfOffBits,SEEK_SET); //offset it to the pixel data

    unsigned char some;

    // for(int c=0;c<(infoheader.biWidth*3);c++){
    //     fread(&some,sizeof(some),1,imageFile);
    // }

    /*for(int j=0;j<(infoheader.biHeight*3);j++){
        for(int k=0;k<(infoheader.biWidth*3);k++){
            fread(&some,sizeof(some),1,imageFile);
            val[j][k] = power(some,1);
            
        }
    }*/

    for(int i =0;i<(infoheader.biHeight*3*infoheader.biWidth*3);i++){
        fread(&some,sizeof(some),1,imageFile);
        val[i] = power(some,1.8);
    }

    fclose(imageFile);  //close the file after reading it



    //trail for writing the file

   // unsigned char padd_val[2] = {0,0};

        FILE *aFile = fopen("flowersCPY.bmp", "wb");   //open the file to write the content

    //checking weather the file is open or not
    if(aFile ==NULL){
        cout<<"Something went wrong! File is not opened"<<endl;
    }
    else{
        cout<<"Opening the file to retrive the contents  to write!"<<endl<<endl;
    


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


    /*for(int j=0;j<(infoheader.biHeight*3);j++){
       for(int k=0;k<(infoheader.biWidth*3);k++){
            fwrite(&val[j][k],sizeof(val[j][k]),1,aFile);
            
        }
        // fwrite(&white,sizeof(white),2,aFile); //padd two bytes at the end of the each row.
    }*/

    for(int p =0;p<(infoheader.biHeight*3*infoheader.biWidth*3);p++){
       fwrite(&val[p],sizeof(val[p]),1,aFile);
    }

    //freeing the memory on the heap
    sbrk(-(infoheader.biHeight*3*infoheader.biWidth*3));
    temp = (unsigned char *)sbrk(0);

    //closing the file
    fclose(aFile);

    }

    return 0;

}