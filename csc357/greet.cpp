#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;



// // int main(){
// //     int *p;
// //     //alocates 5- 4bytes memory on the heap
// //     p = (int*)malloc(5*sizeof(int));

// //     //de referencing the individual element is not required because when
// //     //accessing the individula element pointer automatically gets dereferenced. 
// //     p[0] = 1;
// //     //also increment the pointer itself and dereference it to assign the value at that point.
// //     *(p+1) = 2;        //same as p[1] = 2;

// //        // cout<<(p+1)<<" address of just the pointer of first element ";

// //     return 0;
// // }


// //allocating memory in heap using new and delete keyword


// // int main(){

// //     int *p;

// //     //allocating the memory on the heap
// //     p = new int;
// //     *p =10;
// //     //de-allocating the memory from the heap
// //     delete p;

// //     //allocating the memory on the heap for array
// //     p = new int[10];
// //     p[0]=1;
// //     p[2]=2; //same this as *(p+2) = 2
// //     p[3] =3;

// //     //deallocating the array from the heap
// //     delete[] p;
    
// // }

// void pointr(int **p){
//     int val = 43;
//     *p = &val; // dereferencing the pointer to point to val
//     cout<<"This is the addr of p storing val addr in ptr function: "<<*p<<endl;
//     cout<<"This is the addr of pointer to pointer address in ptr function: "<<p<<endl;
// }

// int main(){

//     int *p = NULL;

//     pointr(&p);

//     cout<<"this is the addr of p in main fucntion: "<<&p<<endl;
//     cout<<"this is the value hold by the p in main function: "<<p<<endl;
// }

// class name{
//     public:
//     int main;
//     float cam;

//     name(){
//         int main1;
//         main =0;
//         cam = 0;
//     }
// };


int main(){
    char text[] = "I sunk the Titanic";
    char *p = text+1;
    cout<<text[5]<<endl;
    cout<<*(p+5)<<endl;
    if(text[5]==*(p+5)){
        cout<<"yes sir"<<endl;
    }
    else{
        cout<<"No sir"<<endl;
    }
}