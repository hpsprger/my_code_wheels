#include<stdio.h>              
                               
int main(void)
{                              
    while(1) {                 
        printf("i am pid:%d ... \n", getpid());
        sleep(1);              
    }
    return 0;
}