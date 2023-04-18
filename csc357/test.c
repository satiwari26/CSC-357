#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(){

    char text[20] = "hello folks!";
    strcat(text," hi!");
    printf("%s",text);

    return 0;
}
