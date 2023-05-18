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

float UserRatio = 0;    //user defined ration, how much do we want to blend the image

unsigned char get_color(unsigned char * colorVal, int x, int y, int imagewidth, int imageHeight, int offsetVal){
    unsigned char interPolatedVal;
    if (x < 0 || x >= imagewidth || y < 0 || y >= imageHeight) {
        // cout<<"something went wrong here! hitting out of bound condition"<<endl;
        return 0; 
    }

    interPolatedVal = colorVal[x*3 + y*imagewidth + offsetVal];   //find the value based on the nearest interger value

    return(interPolatedVal);
}

int main(int argc,char ** argv){

    if(argc !=6){
        printf("%s \n", "not the right number of arguments, try again");
        return 0;
    }

    UserRatio = (float)atof(argv[4]);

    FILE *imageFileLarge = fopen(argv[2], "rb");   //open the large file to read the content

    //checking weather the file is open or not
    if(imageFileLarge ==NULL){
        cout<<"Something went wrong! File is not opened"<<endl;
        return 0;
    }
    else{
        cout<<"Opening the file to retrive the contents"<<endl<<endl;
    }

    tagBITMAPFILEHEADER readHeaderLarge;    //reading content of largeFile
    fread(&readHeaderLarge.bfType,sizeof(readHeaderLarge.bfType),1,imageFileLarge); //have to sequentially read the file content
    fread(&readHeaderLarge.bfSize,sizeof(readHeaderLarge.bfSize),1,imageFileLarge); 
    fread(&readHeaderLarge.bfReserved1,sizeof(readHeaderLarge.bfReserved1),1,imageFileLarge); 
    fread(&readHeaderLarge.bfReserved2,sizeof(readHeaderLarge.bfReserved2),1,imageFileLarge); 
    fread(&readHeaderLarge.bfOffBits,sizeof(readHeaderLarge.bfOffBits),1,imageFileLarge); 

    //sequentially reading the file content from image header

    tagBITMAPINFOHEADER infoheaderLarge;    //reading info of the large File
    fread(&infoheaderLarge.biSize,sizeof(infoheaderLarge.biSize),1,imageFileLarge);
    fread(&infoheaderLarge.biWidth,sizeof(infoheaderLarge.biWidth),1,imageFileLarge);
    fread(&infoheaderLarge.biHeight,sizeof(infoheaderLarge.biHeight),1,imageFileLarge);
    fread(&infoheaderLarge.biPlanes,sizeof(infoheaderLarge.biPlanes),1,imageFileLarge);
    fread(&infoheaderLarge.biBitCount,sizeof(infoheaderLarge.biBitCount),1,imageFileLarge);
    fread(&infoheaderLarge.biCompression,sizeof(infoheaderLarge.biCompression),1,imageFileLarge);
    fread(&infoheaderLarge.biSizeImage,sizeof(infoheaderLarge.biSizeImage),1,imageFileLarge);
    fread(&infoheaderLarge.biXPelsPerMeter,sizeof(infoheaderLarge.biXPelsPerMeter),1,imageFileLarge);
    fread(&infoheaderLarge.biYPelsPerMeter,sizeof(infoheaderLarge.biYPelsPerMeter),1,imageFileLarge);
    fread(&infoheaderLarge.biClrUsed,sizeof(infoheaderLarge.biClrUsed),1,imageFileLarge);
    fread(&infoheaderLarge.biClrImportant,sizeof(infoheaderLarge.biClrImportant),1,imageFileLarge);

    unsigned char * valLarge, *tempLarge;


    //adding padding
    LONG widthBytes,padding,real_width;

    widthBytes = (infoheaderLarge.biWidth*3);
    if(widthBytes%4 !=0)
    {
    padding = 4-(widthBytes%4);
    }
    // cout<<padding<<endl;
    real_width = widthBytes+padding;
    //adding padding

    //need to store the high rez data so that we can use it later when blending the two images
    //assigning block of memory on heap and using val to keep track of prgm brk
    valLarge = (unsigned char *)malloc((real_width*infoheaderLarge.biHeight));



    fseek(imageFileLarge,readHeaderLarge.bfOffBits,SEEK_SET); //offset it to the pixel data
    unsigned char some;

    for(int i =0;i<(real_width*infoheaderLarge.biHeight);i++){
        fread(&some,sizeof(some),1,imageFileLarge);
        valLarge[i] = some; //store the large image data sequentially
    }

    fclose(imageFileLarge);  //close the file after reading it

    //******************************************************************************************//

    FILE *imageFileSmall = fopen(argv[3], "rb");   //open the smaller file to read the content

     //checking weather the file is open or not
    if(imageFileSmall ==NULL){
        cout<<"Something went wrong! File is not opened"<<endl;
        return 0;
    }
    else{
        cout<<"Opening the file to retrive the contents"<<endl<<endl;
    }

    tagBITMAPFILEHEADER readHeaderSmall;    //reading content of largeFile
    fread(&readHeaderSmall.bfType,sizeof(readHeaderSmall.bfType),1,imageFileSmall); //have to sequentially read the file content
    fread(&readHeaderSmall.bfSize,sizeof(readHeaderSmall.bfSize),1,imageFileSmall); 
    fread(&readHeaderSmall.bfReserved1,sizeof(readHeaderSmall.bfReserved1),1,imageFileSmall); 
    fread(&readHeaderSmall.bfReserved2,sizeof(readHeaderSmall.bfReserved2),1,imageFileSmall); 
    fread(&readHeaderSmall.bfOffBits,sizeof(readHeaderSmall.bfOffBits),1,imageFileSmall); 

    //sequentially reading the file content from image header

    tagBITMAPINFOHEADER infoheaderSmall;    //reading info of the small File
    fread(&infoheaderSmall.biSize,sizeof(infoheaderSmall.biSize),1,imageFileSmall);
    fread(&infoheaderSmall.biWidth,sizeof(infoheaderSmall.biWidth),1,imageFileSmall);
    fread(&infoheaderSmall.biHeight,sizeof(infoheaderSmall.biHeight),1,imageFileSmall);
    fread(&infoheaderSmall.biPlanes,sizeof(infoheaderSmall.biPlanes),1,imageFileSmall);
    fread(&infoheaderSmall.biBitCount,sizeof(infoheaderSmall.biBitCount),1,imageFileSmall);
    fread(&infoheaderSmall.biCompression,sizeof(infoheaderSmall.biCompression),1,imageFileSmall);
    fread(&infoheaderSmall.biSizeImage,sizeof(infoheaderSmall.biSizeImage),1,imageFileSmall);
    fread(&infoheaderSmall.biXPelsPerMeter,sizeof(infoheaderSmall.biXPelsPerMeter),1,imageFileSmall);
    fread(&infoheaderSmall.biYPelsPerMeter,sizeof(infoheaderSmall.biYPelsPerMeter),1,imageFileSmall);
    fread(&infoheaderSmall.biClrUsed,sizeof(infoheaderSmall.biClrUsed),1,imageFileSmall);
    fread(&infoheaderSmall.biClrImportant,sizeof(infoheaderSmall.biClrImportant),1,imageFileSmall);

    unsigned char * valSmall, *tempSmall;


    //adding padding
    LONG widthBytes2,padding2,real_width2;

    widthBytes2 = (infoheaderSmall.biWidth*3);
    if(widthBytes2%4 !=0){
    padding2 = 4-(widthBytes2%4);
    }
    // cout<<padding2<<endl;
    real_width2 = widthBytes2+padding2;
    //adding padding

    //need to store the small rez data so that we can use it later when blending the two images
    //assigning block of memory on heap and using val to keep track of prgm brk
    valSmall = (unsigned char *)malloc((real_width2*infoheaderSmall.biHeight));


    fseek(imageFileSmall,readHeaderSmall.bfOffBits,SEEK_SET); //offset it to the pixel data
    unsigned char some2;

    for(int i =0;i<(real_width2*infoheaderSmall.biHeight);i++){
        fread(&some2,sizeof(some2),1,imageFileSmall);
        valSmall[i] = some2; //store the small image data sequentially
    }
    
    fclose(imageFileSmall);  //close the file after reading it

    //*********************************************************************************************//

    //creating high rez lion image now!! using interpolation

    FILE *aFile = fopen(argv[5], "wb");   //open the file to write the content

    //checking weather the file is open or not
    if(aFile ==NULL){
        cout<<"Something went wrong! File is not opened"<<endl;
        return 0;
    }
    else{
        cout<<"Opening the file to retrive the contents  to write!"<<endl<<endl;
    }

    if(infoheaderLarge.biSizeImage>infoheaderSmall.biSizeImage){    //if the first file size is bigger

        fwrite(&readHeaderLarge.bfType,sizeof(readHeaderLarge.bfType),1,aFile); //have to sequentially write the file content and would be same as larger file
        fwrite(&readHeaderLarge.bfSize,sizeof(readHeaderLarge.bfSize),1,aFile); 
        fwrite(&readHeaderLarge.bfReserved1,sizeof(readHeaderLarge.bfReserved1),1,aFile); 
        fwrite(&readHeaderLarge.bfReserved2,sizeof(readHeaderLarge.bfReserved2),1,aFile); 
        fwrite(&readHeaderLarge.bfOffBits,sizeof(readHeaderLarge.bfOffBits),1,aFile); 

        //sequentially writing the file content from image header

        fwrite(&infoheaderLarge.biSize,sizeof(infoheaderLarge.biSize),1,aFile);
        fwrite(&infoheaderLarge.biWidth,sizeof(infoheaderLarge.biWidth),1,aFile);
        fwrite(&infoheaderLarge.biHeight,sizeof(infoheaderLarge.biHeight),1,aFile);
        fwrite(&infoheaderLarge.biPlanes,sizeof(infoheaderLarge.biPlanes),1,aFile);
        fwrite(&infoheaderLarge.biBitCount,sizeof(infoheaderLarge.biBitCount),1,aFile);
        fwrite(&infoheaderLarge.biCompression,sizeof(infoheaderLarge.biCompression),1,aFile);
        fwrite(&infoheaderLarge.biSizeImage,sizeof(infoheaderLarge.biSizeImage),1,aFile);
        fwrite(&infoheaderLarge.biXPelsPerMeter,sizeof(infoheaderLarge.biXPelsPerMeter),1,aFile);
        fwrite(&infoheaderLarge.biYPelsPerMeter,sizeof(infoheaderLarge.biYPelsPerMeter),1,aFile);
        fwrite(&infoheaderLarge.biClrUsed,sizeof(infoheaderLarge.biClrUsed),1,aFile);
        fwrite(&infoheaderLarge.biClrImportant,sizeof(infoheaderLarge.biClrImportant),1,aFile);


        fseek(aFile,readHeaderLarge.bfOffBits,SEEK_SET);    //offset it to write the content.

        unsigned char * valHighRez, *tempHighRez;
        //adding padding
        LONG widthBytes3,padding3,real_width3;

        widthBytes3 = (infoheaderLarge.biWidth*3);
        if(widthBytes3%4 !=0){  
        padding3 = 4-(widthBytes3%4);   
        }
        // cout<<padding3<<endl;
        real_width3 = widthBytes3+padding3;
        //adding padding

        //assigning block of memory on heap for highRez and using val to keep track of prgm brk
        valHighRez = (unsigned char *)malloc((real_width3*infoheaderLarge.biHeight));

        
        //looping through the high rez and using interpolation algorithm to create high rez image:
        for(int x =0;x<infoheaderLarge.biWidth;x++){
            for(int y=0;y<infoheaderLarge.biHeight;y++){
                float lowerX = (float)((x)*((real_width2)/((float)real_width)));
                float lowerY = (float)((y)*((infoheaderSmall.biHeight)/((float)infoheaderLarge.biHeight)));

                //nearest integer coordinates
                int x1,x2,y1,y2;
                float dx,dy;
                // int x coordinates
                x1 = (int)lowerX;
                dx = abs(lowerX-(float)x1);
                x2 = x1+1;
                //int y coordinates
                y1 = (int)lowerY;
                dy = abs(lowerY - (float)y1);
                y2 = y1+1;

                //call the get_color function
                unsigned char red[4];
                unsigned char green[4];
                unsigned char blue[4];

                //bilinear blue
                blue[0] = get_color(valSmall, x1,y2, real_width2, infoheaderSmall.biHeight,0);    //blue left-upper
                blue[1] = get_color(valSmall, x2,y2, real_width2, infoheaderSmall.biHeight,0);    //blue Right-upper
                blue[2] = get_color(valSmall, x1,y1, real_width2, infoheaderSmall.biHeight,0);    //blue left-lower
                blue[3] = get_color(valSmall, x2,y1, real_width2, infoheaderSmall.biHeight,0);    //blue right-lower

                //interpolate
                unsigned char blue_left = blue[0] *(1-dy) + blue[2]*dy;
                unsigned char blue_right = blue[1]*(1-dy) + blue[3]*dy;
                //storing blue in the highRez list
                valHighRez[x*3+(y*real_width3)+0] = blue_left* (1-dx) + blue_right*dx;
                // valHighRez[x*3+(y*real_width)+0] = valLarge[x*3+(y*real_width)+0];

                //bilinear green
                green[0] = get_color(valSmall, x1,y2, real_width2, infoheaderSmall.biHeight,1);    //green left-upper
                green[1] = get_color(valSmall, x2,y2, real_width2, infoheaderSmall.biHeight,1);    //green Right-upper
                green[2] = get_color(valSmall, x1,y1, real_width2, infoheaderSmall.biHeight,1);    //green left-lower
                green[3] = get_color(valSmall, x2,y1, real_width2, infoheaderSmall.biHeight,1);    //green right-lower

                //interpolate
                unsigned char green_left = green[0] *(1-dy) + green[2]*dy;
                unsigned char green_right = green[1]*(1-dy) + green[3]*dy;
                //storing blue in the highRez list
                valHighRez[x*3+(y*real_width3)+1] = green_left* (1-dx) + green_right*dx;
                // valHighRez[x*3+(y*real_width)+1] = valLarge[x*3+(y*real_width)+1];

                //bilinear red
                red[0] = get_color(valSmall, x1,y2, real_width2, infoheaderSmall.biHeight,2);    //red left-upper
                red[1] = get_color(valSmall, x2,y2, real_width2, infoheaderSmall.biHeight,2);    //red Right-upper
                red[2] = get_color(valSmall, x1,y1, real_width2, infoheaderSmall.biHeight,2);    //red left-lower
                red[3] = get_color(valSmall, x2,y1, real_width2, infoheaderSmall.biHeight,2);    //red right-lower

                //interpolate
                unsigned char red_left = red[0] *(1-dy) + red[2]*dy;
                unsigned char red_right = red[1]*(1-dy) + red[3]*dy;
                //storing blue in the highRez list
                valHighRez[x*3+(y*real_width3)+2] = red_left* (1-dx) + red_right*dx;
                // valHighRez[x*3+(y*real_width)+2] = valLarge[x*3+(y*real_width)+2];
            }
        }

        for(int p =0;p<(real_width3*infoheaderLarge.biHeight);p++){
            valHighRez[p] = UserRatio*valLarge[p]+valHighRez[p]*(1-UserRatio);
        fwrite(&valHighRez[p],sizeof(valHighRez[p]),1,aFile);
        }

    }
    else{   //second file size is bigger

        fwrite(&readHeaderSmall.bfType,sizeof(readHeaderSmall.bfType),1,aFile); //have to sequentially write the file content and would be same as larger file
        fwrite(&readHeaderSmall.bfSize,sizeof(readHeaderSmall.bfSize),1,aFile); 
        fwrite(&readHeaderSmall.bfReserved1,sizeof(readHeaderSmall.bfReserved1),1,aFile); 
        fwrite(&readHeaderSmall.bfReserved2,sizeof(readHeaderSmall.bfReserved2),1,aFile); 
        fwrite(&readHeaderSmall.bfOffBits,sizeof(readHeaderSmall.bfOffBits),1,aFile); 

        //sequentially writing the file content from image header

        fwrite(&infoheaderSmall.biSize,sizeof(infoheaderSmall.biSize),1,aFile);
        fwrite(&infoheaderSmall.biWidth,sizeof(infoheaderSmall.biWidth),1,aFile);
        fwrite(&infoheaderSmall.biHeight,sizeof(infoheaderSmall.biHeight),1,aFile);
        fwrite(&infoheaderSmall.biPlanes,sizeof(infoheaderSmall.biPlanes),1,aFile);
        fwrite(&infoheaderSmall.biBitCount,sizeof(infoheaderSmall.biBitCount),1,aFile);
        fwrite(&infoheaderSmall.biCompression,sizeof(infoheaderSmall.biCompression),1,aFile);
        fwrite(&infoheaderSmall.biSizeImage,sizeof(infoheaderSmall.biSizeImage),1,aFile);
        fwrite(&infoheaderSmall.biXPelsPerMeter,sizeof(infoheaderSmall.biXPelsPerMeter),1,aFile);
        fwrite(&infoheaderSmall.biYPelsPerMeter,sizeof(infoheaderSmall.biYPelsPerMeter),1,aFile);
        fwrite(&infoheaderSmall.biClrUsed,sizeof(infoheaderSmall.biClrUsed),1,aFile);
        fwrite(&infoheaderSmall.biClrImportant,sizeof(infoheaderSmall.biClrImportant),1,aFile);


        fseek(aFile,readHeaderSmall.bfOffBits,SEEK_SET);    //offset it to write the content.

        unsigned char * valHighRez, *tempHighRez;
        //adding padding
        LONG widthBytes3,padding3,real_width3;

        widthBytes3 = (infoheaderSmall.biWidth*3);
        if(widthBytes3%4 !=0){  
        padding3 = 4-(widthBytes3%4);   
        }
        // cout<<padding3<<endl;
        real_width3 = widthBytes3+padding3;
        //adding padding

        //assigning block of memory on heap for highRez and using val to keep track of prgm brk
        valHighRez = (unsigned char *)malloc((real_width3*infoheaderSmall.biHeight));

        
        //looping through the high rez and using interpolation algorithm to create high rez image:
        for(int x =0;x<infoheaderSmall.biWidth;x++){
            for(int y=0;y<infoheaderSmall.biHeight;y++){
                float lowerX = (float)((x)*((real_width)/((float)real_width2)));
                float lowerY = (float)((y)*((infoheaderLarge.biHeight)/((float)infoheaderSmall.biHeight)));

                //nearest integer coordinates
                int x1,x2,y1,y2;
                float dx,dy;
                // int x coordinates
                x1 = (int)lowerX;
                dx = abs(lowerX-(float)x1);
                x2 = x1+1;
                //int y coordinates
                y1 = (int)lowerY;
                dy = abs(lowerY - (float)y1);
                y2 = y1+1;

                //call the get_color function
                unsigned char red[4];
                unsigned char green[4];
                unsigned char blue[4];

                //bilinear blue
                blue[0] = get_color(valLarge, x1,y2, real_width, infoheaderLarge.biHeight,0);    //blue left-upper
                blue[1] = get_color(valLarge, x2,y2, real_width, infoheaderLarge.biHeight,0);    //blue Right-upper
                blue[2] = get_color(valLarge, x1,y1, real_width, infoheaderLarge.biHeight,0);    //blue left-lower
                blue[3] = get_color(valLarge, x2,y1, real_width, infoheaderLarge.biHeight,0);    //blue right-lower

                //interpolate
                unsigned char blue_left = blue[0] *(1-dy) + blue[2]*dy;
                unsigned char blue_right = blue[1]*(1-dy) + blue[3]*dy;
                //storing blue in the highRez list
                valHighRez[x*3+(y*real_width3)+0] = blue_left* (1-dx) + blue_right*dx;
                // valHighRez[x*3+(y*real_width)+0] = valLarge[x*3+(y*real_width)+0];

                //bilinear green
                green[0] = get_color(valLarge, x1,y2, real_width, infoheaderLarge.biHeight,1);    //green left-upper
                green[1] = get_color(valLarge, x2,y2, real_width, infoheaderLarge.biHeight,1);    //green Right-upper
                green[2] = get_color(valLarge, x1,y1, real_width, infoheaderLarge.biHeight,1);    //green left-lower
                green[3] = get_color(valLarge, x2,y1, real_width, infoheaderLarge.biHeight,1);    //green right-lower

                //interpolate
                unsigned char green_left = green[0] *(1-dy) + green[2]*dy;
                unsigned char green_right = green[1]*(1-dy) + green[3]*dy;
                //storing blue in the highRez list
                valHighRez[x*3+(y*real_width3)+1] = green_left* (1-dx) + green_right*dx;
                // valHighRez[x*3+(y*real_width)+1] = valLarge[x*3+(y*real_width)+1];

                //bilinear red
                red[0] = get_color(valLarge, x1,y2, real_width, infoheaderLarge.biHeight,2);    //red left-upper
                red[1] = get_color(valLarge, x2,y2, real_width, infoheaderLarge.biHeight,2);    //red Right-upper
                red[2] = get_color(valLarge, x1,y1, real_width, infoheaderLarge.biHeight,2);    //red left-lower
                red[3] = get_color(valLarge, x2,y1, real_width, infoheaderLarge.biHeight,2);    //red right-lower

                //interpolate
                unsigned char red_left = red[0] *(1-dy) + red[2]*dy;
                unsigned char red_right = red[1]*(1-dy) + red[3]*dy;
                //storing blue in the highRez list
                valHighRez[x*3+(y*real_width3)+2] = red_left* (1-dx) + red_right*dx;
                // valHighRez[x*3+(y*real_width)+2] = valLarge[x*3+(y*real_width)+2];
            }
        }

        for(int p =0;p<(real_width3*infoheaderSmall.biHeight);p++){
            valHighRez[p] = UserRatio*valSmall[p]+valHighRez[p]*(1-UserRatio);
        fwrite(&valHighRez[p],sizeof(valHighRez[p]),1,aFile);
        }

    }

     //closing the file
    fclose(aFile);

    return 0;
}