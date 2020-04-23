#include <stdio.h>


int main(void)
{
    int a, b, c; 
    sscanf("2006:03:18", "%d:%d:%d", &a, &b, &c);
    printf("%d--%d--%d \n",a,b,c);
    return 0;
}
//2006--3--18

