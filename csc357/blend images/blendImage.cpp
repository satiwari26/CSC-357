#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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


int main(){

    FILE *imageFile = fopen("flowers.bmp", "rb");   //open the file to read the content

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



    // unsigned char **val = new unsigned char*[1473];

    // for(int i=0;i<1473;i++){
    //     val[i] = new unsigned char[2358];
    // }


    unsigned char val[1473][2358];


    fseek(imageFile,readHeader.bfOffBits,SEEK_SET); //offset it to the pixel data

    unsigned char some;

    // for(int c=0;c<2358;c++){
    //     fread(&some,sizeof(some),1,imageFile);
    // }

    for(int j=0;j<1473;j++){
        for(int k=0;k<2358;k++){
            fread(&some,sizeof(some),1,imageFile);
            val[j][k] = some;
            
        }
    }
    fclose(imageFile);  //close the file after reading it




    //trail for writing the file

    unsigned char padd_val[2] = {0,0};

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

    for(int j=0;j<1473;j++){
       for(int k=0;k<2358;k++){
            fwrite(&val[j][k],sizeof(val[j][k]),1,aFile);
            
        }
        //fwrite(&padd_val,sizeof(padd_val),2,aFile); //padd two bytes at the end of the each row.
    }
    fclose(aFile);


    }

    return 0;

}