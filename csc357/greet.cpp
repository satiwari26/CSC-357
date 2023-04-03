#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main(){
    int *p;
    //alocates 5- 4bytes memory on the heap
    p = (int*)malloc(5*sizeof(int));

    //de referencing the individual element is not required because when
    //accessing the individula element pointer automatically gets dereferenced. 
    p[0] = 1;
    //also increment the pointer itself and dereference it to assign the value at that point.
    *(p+1) = 2;        //same as p[1] = 2;

    cout<<p<<" addr of pointing"<<endl;
    cout<<&p<<" addr of pointer"<<endl;

       // cout<<(p+1)<<" address of just the pointer of first element ";

    return 0;
}

