#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>



int main(){

    char str[] = "GET /doc/index.html";
    char *buffer;  

    buffer = strtok(str, " "); 
    printf("%s", buffer); 

    int x = 2;
    printf("detter er int %i\n", x); 
    int *p_x = &x;  // Put the address of the x variable into the pointer p_x
    *p_x = 4;       // Change the memory at the address in p_x to be 4

    printf("detter er int %i\n", x); 

    


    return 0; 
}