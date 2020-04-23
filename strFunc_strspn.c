#include <stdio.h>
#include <string.h>

int main ()
{
    int len;
    const char str1[] = "ABCDEFG019874";
    const char str2[] = "ABCD";
    const char str3[] = "ABCDEFG019874";
    const char str4[] = "EFG";
    const char str5[] = "ABCDEFG019874";
    const char str6[] = "AB";
    const char str7[] = "ABCDEFG019874";
    const char str8[] = "ABEF";
    
    len = strspn(str1, str2);
    printf("str1 初始段匹配长度 %d\n", len );//4
    
    len = strspn(str3, str4);
    printf("str3 初始段匹配长度 %d\n", len );//0

    len = strspn(str5, str6);
    printf("str5 初始段匹配长度 %d\n", len );//2

    len = strspn(str7, str8);
    printf("str7 初始段匹配长度 %d\n", len );//2

    return(0);
}

#if 0
main()
{
    char *str="Linux was first developed for 386/486-based pcs.";
    printf("%d\n",strspn(str,"Linux"));
    printf("%d\n",strspn(str,"/-"));
    printf("%d\n",strspn(str,"1234567890"));
}
运行结果：
5
0
0
#endif

