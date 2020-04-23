#include <stdio.h>
#include <ctype.h>
int main ()
{
    int i = 0;
    char str[] = "C++ Java Python C# Linux Golang Shell\n";
    char c;
    while(str[i])
    {
        c = str[i];
        if(isupper(c)) {
            printf("\n %c--", c);
            c = tolower(c);
            printf("%c\n", c);
        }
        putchar(c);
        i++;
    }
    return 0;
}

/*
 *
 *  C--c
 *  c++ 
 *  J--j
 *  java 
 *  P--p
 *  python 
 *  C--c
 *  c# 
 *  L--l
 *  linux 
 *  G--g
 *  golang 
 *  S--s
 *  shell
 * 
 * */
