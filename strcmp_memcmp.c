int strcmp(const char *str1, const char *str2)
    参数
    str1 -- 要进行比较的第一个字符串。
    str2 -- 要进行比较的第二个字符串。
    返回值
    该函数返回值如下：

    如果返回值小于 0，则表示 str1 小于 str2。
    如果返回值大于 0，则表示 str1 大于 str2。
    如果返回值等于 0，则表示 str1 等于 str2。

    比较规则：字符串比较函数strcmp的比较，两个字符串自左向右逐个字符相比（按ASCII值大小相比较），直到出现不同的字符或遇'\0'为止。
    基本形式为strcmp(str1,str2)，
    若str1=str2，则返回零；
    若str1<str2，则返回负数；
    若str1>str2，则返回正数


memcmp函数的原型为 
int memcmp(const void *str1, const void *str2, size_t n));
其功能是把存储区 str1 和存储区 str2 的前 n 个字节进行比较。
该函数是按字节比较的，位于string.h


#include<stdio.h>

int main(void)
{
    int result;

    result=strcmp("a", "A");
    printf("a=%u A=%u strcmp(\"a\", \"A\")=%d\n",'a','A',result);
    result=strcmp("A", "a");
    printf("a=%u A=%u strcmp(\"A\", \"a\")=%d\n",'a','A',result);
    result=strcmp("2020:0101:01", "2020:0100:02");
    printf("2020:0101:01   2020:0100:02 ==> %d\n",result);
    result=strcmp("2020:0101:01", "2020:0101:02");
    printf("2020:0101:01   2020:0101:02 ==> %d\n",result);

    result=memcmp("a", "A", 1);
    printf("a=%u A=%u memcmp(\"a\", \"A\")=%d\n",'a','A',result);
    result=memcmp("A", "a", 1);
    printf("a=%u A=%u memcmp(\"A\", \"a\")=%d\n",'a','A',result);
    result=memcmp("2020:0101:01", "2020:0100:02", 12);
    printf("memcpy 2020:0101:01   2020:0100:02 ==> %d\n",result);
    result=memcmp("2020:0101:01", "2020:0101:02", 12);
    printf("memcpy 2020:0101:01   2020:0101:02 ==> %d\n",result);

    result=memcmp("2020:0103:01", "2020:0100:02", 12);
    printf("memcpy 2020:0103:01   2020:0100:02 ==> %d\n",result);
    result=memcmp("2020:0101:01", "2020:0108:02", 12);
    printf("memcpy 2020:0101:01   2020:0108:02 ==> %d\n",result);

    return 0;
}



hpsp@hpsplnx:~/rock_space/my_code_tst$ ./a.out 
a=97 A=65 strcmp("a", "A")=1
a=97 A=65 strcmp("A", "a")=-1
2020:0101:01   2020:0100:02 ==> 1
2020:0101:01   2020:0101:02 ==> -1
a=97 A=65 memcmp("a", "A")=32
a=97 A=65 memcmp("A", "a")=-32
memcpy 2020:0101:01   2020:0100:02 ==> 1
memcpy 2020:0101:01   2020:0101:02 ==> -1
memcpy 2020:0103:01   2020:0100:02 ==> 3
memcpy 2020:0101:01   2020:0108:02 ==> -7


strcmp("2017:01:01:23:00:59","2017:01:01:22:10:59")   ==> 1
strcmp("2017:01:01:23:00:59","2017:01:01:23:01:59")   ==> -1



力扣635

typedef struct Msg_ {
    int id;
    char timestamp[30];
    struct Msg_ *next;
} Msg;

typedef struct {
    Msg *head;
} LogSystem;

LogSystem* logSystemCreate() {
    return calloc(1, sizeof(LogSystem));
}

void logSystemPut(LogSystem* obj, int id, char * timestamp) {
    Msg *pMag = (Msg *)calloc(1, sizeof(Msg));
    if ((pMag == 0) || (obj == 0)) {
        return;
    }
    pMag->id = id;
    strcpy(pMag->timestamp, timestamp);
    pMag->next = obj->head;
    obj->head = pMag;
}

int* logSystemRetrieve(LogSystem* obj, char * s, char * e, char * gra, int* retSize) {
    Msg *pMag = obj->head;
    Msg *pTmp = obj->head;
    int *result=(int *)calloc(10000, sizeof(int));
    int count = 0;
    int cmpLen;

    if (strcmp(gra, "Year") == 0) {
        cmpLen = 4;
    } else if (strcmp(gra, "Month") == 0) {
        cmpLen = 7;    
    } else if (strcmp(gra, "Day") == 0) {
        cmpLen = 10;
    } else if (strcmp(gra, "Hour") == 0) {
        cmpLen = 13;    
    } else if (strcmp(gra, "Minute") == 0) {
        cmpLen = 16;
    } else if (strcmp(gra, "Second") == 0) {
        cmpLen = 19;    
    } else {
        cmpLen = 0;
    }

    while (pMag) {
        pTmp = pMag->next;
        if ((memcmp(s, pMag->timestamp, cmpLen) <= 0) && (memcmp(e, pMag->timestamp, cmpLen) >= 0)) {
            result[count++] = pMag->id;
        }
        pMag = pTmp;
    } 

    *retSize = count;
    return  result; 
}

void logSystemFree(LogSystem* obj) {
    Msg *pMag = obj->head;
    Msg *pTmp = obj->head;
    while (pMag) {
        pTmp = pMag->next;
        free(pMag);
        pMag = pTmp;
    }
    free(obj);
}

/**
 *  * Your LogSystem struct will be instantiated and called as such:
 *   * LogSystem* obj = logSystemCreate();
 *    * logSystemPut(obj, id, timestamp);
 *     
 *      * int* param_2 = logSystemRetrieve(obj, s, e, gra, retSize);
 *       
 *        * logSystemFree(obj);
 *        */



