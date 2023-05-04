#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(){

    char text[20] = "hello world!";
    char text2[20] = "my name ";

    char **p;
     *p = text;
    //   *(p+1) = text2;

     printf("%s \n",*p);
      //printf("%s \n",*(p+1));

    return 0;
}
