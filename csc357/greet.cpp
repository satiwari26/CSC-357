#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;



// int main(){
//     int *p;
//     //alocates 5- 4bytes memory on the heap
//     p = (int*)malloc(5*sizeof(int));

//     //de referencing the individual element is not required because when
//     //accessing the individula element pointer automatically gets dereferenced. 
//     p[0] = 1;
//     //also increment the pointer itself and dereference it to assign the value at that point.
//     *(p+1) = 2;        //same as p[1] = 2;

//        // cout<<(p+1)<<" address of just the pointer of first element ";

//     return 0;
// }


//allocating memory in heap using new and delete keyword


// int main(){

//     int *p;

//     //allocating the memory on the heap
//     p = new int;
//     *p =10;
//     //de-allocating the memory from the heap
//     delete p;

//     //allocating the memory on the heap for array
//     p = new int[10];
//     p[0]=1;
//     p[2]=2; //same this as *(p+2) = 2
//     p[3] =3;

//     //deallocating the array from the heap
//     delete[] p;
    
// }


int main(){
    char text[5]; //allocating 5 byte memory
    //string a;
    //cout<<sizeof(a)<<endl;

    text[0] = 'h'; //(*text) = 'h'
    text[1] = 'e';
    text[2] = 'l';
    text[3] = 'l';
    text[4] = 'o';
    //text[5] = 'w';

    printf("%s \n",text);
    //cout<<text<<endl;
}

