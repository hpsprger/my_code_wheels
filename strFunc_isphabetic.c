#include<stdio.h>
#include<string.h>
#include <ctype.h>

int main()
{
    int i = 0;
    char str[] = "C++ Java C#";
    while (str[i])
    {
        if (isalpha(str[i])) {
            printf("%c is alphabetic\n", str[i]);
        }
        else {
            printf("%c is not alphabetic\n", str[i]);
        }
        i++;
    }
    return 0;
}

