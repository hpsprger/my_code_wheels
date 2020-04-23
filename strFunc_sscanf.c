#include <stdio.h>


int main(void)
{
    int a, b, c; 
    sscanf("2006:03:18", "%d:%d:%d", &a, &b, &c);
    printf("%d--%d--%d \n",a,b,c);
    return 0;
}
//2006--3--18


s   字符串。这将读取连续字符，直到遇到一个空格字符（空格字符可以是空白、换行和制表符）。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int day, year;
    char weekday[20], month[20], dtm[100];

    strcpy( dtm, "Saturday March 25 1989" );
    sscanf( dtm, "%s %s %d  %d", weekday, month, &day, &year );

    printf("%s %d, %d = %s\n", month, day, year, weekday );

    return(0);
}
