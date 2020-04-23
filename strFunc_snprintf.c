#include <stdio.h> 

int main() 
{ 
    char buffer[50]; 
    char* s = "runoobcom"; 

    int j = snprintf(buffer, 6, "%s\n", s); 

    printf("string:\n%s\ncharacter count = %d\n", buffer, j); 

    return 0; 
} 

/*
 *string:
 runoo
 character count = 10
 * */

