#include<stdio.h>

int main()
{
    char str[]="1776ad";
    int  year;

    printf("%d \n", isdigit(str[0]));//printf 1   
    if(isdigit(str[0]))    
    {
        year = atoi(str);
        printf ("The year that followed %d was %d.\n", year, year+1);
    }
    return 0;
}


