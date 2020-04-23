#include<string.h>
#include<stdio.h>

/* strtok("abc,def,ghi",",")，最后可以分割成为abc def ghi.尤其在点分十进制的IP中提取应用较多 */

/*
   strtok()函数详解！
   1.定义
   分解字符串为一组字符串。s为要分解的字符，delim为分隔符字符（如果传入字符串，则传入的字符串中每个字符均为分割符）。首次调用时，s指向要分解的字符串，之后再次调用要把s设成NULL。在头文件#include<string.h>中。
   2.原型
   char *strtok(char s[], const char *delim);
   3.说明
   （1）当strtok()在参数s的字符串中发现参数delim中包含的分割字符时,则会将该字符改为\0 字符。在第一次调用时，strtok()必需给予参数s字符串，往后的调用则将参数s设置成NULL。每次调用成功则返回指向被分割出片段的指针。
   （2）返回值

   从s开头开始的一个个被分割的串。当s中的字符查找到末尾时，返回NULL。如果查找不到delim中的字符时，返回当前strtok的字符串的指针。所有delim中包含的字符都会被滤掉，并将被滤掉的地方设为一处分割的节点。

   （3）需要注意的是，使用该函数进行字符串分割时，会破坏被分解字符串的完整，调用前和调用后的s已经不一样了。第一次分割之后，原字符串str是分割完成之后的第一个字符串，剩余的字符串存储在一个静态变量中，因此多线程同时访问该静态变量时，则会出现错误。

   4.使用
   strtok函数会破坏被分解字符串的完整，调用前和调用后的s已经不一样了。如果要保持原字符串的完整，可以使用strchr和sscanf的组合等。

   5、strtok_s函数
   strtok_s是windows下的一个分割字符串安全函数，其函数原型如下：
   char *strtok_s( char *strToken, const char *strDelimit, char **buf);
   这个函数将剩余的字符串存储在buf变量中，而不是静态变量中，从而保证了安全性。
   6、strtok_r函数
   strtok_s函数是linux下分割字符串的安全函数，函数声明如下：
   char *strtok_r(char *str, const char *delim, char **saveptr);
   该函数也会破坏带分解字符串的完整性，但是其将剩余的字符串保存在saveptr变量中，保证了安全性。

 */

int main()  
{  
    char str[]="ab,cd,ef";  
    char *ptr;  
    char *p;  
    printf("before strtok:  str=%s\n",str);  
    printf("begin:\n");  
    ptr = strtok_r(str, ",", &p);  
    while(ptr != NULL){  
        printf("str=%s\n",str);  
        printf("ptr=%s\n",ptr);  
        ptr = strtok_r(NULL, ",", &p);  
    }  
    return 0;  
}


#if 0
int main(void)
{
    char Str[50] = {"abc,def,ghi"};
    char *p;
    
    p = strtok(Str, ",");
    //if (p) 
        printf("%p--%s \n", p, p);
    //else 
    //    printf("null \n");

    return 0;
}
#endif 

#if 0
int main(void)
{
    char input[16]="abc,d";
    char*p;
    /*strtok places a NULL terminator
     *     infront of the token,if found*/
    p=strtok(input,",");
    if(p)
        printf("%s\n",p);
    /*Asecond call to strtok using a NULL
     *         as the first parameter returns a pointer
     *                 to the character following the token*/
    p=strtok(NULL,",");
    if(p)
        printf("%s\n",p);
    return 0;
}
#endif
