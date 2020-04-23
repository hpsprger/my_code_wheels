atoi()函数
atoi()原型:  int atoi(const char *str );

函数功能：把字符串转换成整型数。

参数str：要进行转换的字符串 ==> atoi会一直转换，直到遇到str中第一个非数字的字符 就停止转换

返回值：每个函数返回 int 值，此值由将输入字符作为数字解析而生成。 如果该输入无法转换为该类型的值，则atoi的返回值为 0。

注意：使用该函数时要注意atoi返回的是int类型，注意输入str的范围不要超出int类型的范围。


long int strtol(const char *nptr,char **endptr,int base);
nptr为待转换的字符串
endptr为本次转换结束地址的下一个字节的地址
参数base范围从2至36，或0。参数base代表采用的进制方式，如base值为10则采用10进制，若base值为16则采用16进制等。
当base值为0时则是采用10进制做转换，但遇到如’0x’前置字符则会使用16进制做转换、遇到’0’前置字符而不是’0x’的时候会使用8进制做转换。
一开始strtol()会扫描参数nptr字符串，跳过前面的空格字符，直到遇上数字或正负符号才开始做转换，再遇到非数字或字符串结束时('\0')结束转换，并将结果返回。
若参数endptr不为NULL，则会将遇到不合条件而终止的nptr中的字符指针由endptr返回；若参数endptr为NULL，则会不返回非法字符串。


通过strtol从字符串的首地址处提取整数子字符串并转为long int型返回
比如:strtol(curPos, &nextPos, 10);
如果字符串的首地址不是整数子字符串开头的，那么strtol函数的curPos就会等于nextPos,通过这两个参数的判断我们可以知道当前的这个字符串是不是以数字子字符串开始的，下面的代码就是这个来处理的

力扣640题目
char * solveEquation(char * equation){

    unsigned int i = 0;
    char nextChar;
    char *curPos;
    char *nextPos;
    char *resultBuffer;
    char symRevert = 1; //1 表示=号左边  -1表示=号右边
    int xCount = 0; //x 前的系数
    long int sum = 0; //=号右边的值
    long int curData;
    long int xValue;   
    char curXSym = 1; //1正 -1负


    if (equation == 0) {
        return 0;
    }

    if (strlen(equation) == 0) {
        return 0;
    }

    resultBuffer = (char *)calloc(100, sizeof(char));

    curPos = equation;

    while (*curPos != '\0') {

        curData = strtol(curPos, &nextPos, 10);
        curData = curData * symRevert; 



        if (curPos == nextPos) { //针对 x +x -x = 这种符号场景，取不出整数来
            if ((curPos[0] == '-') && (curPos[1] == 'x')) { //-x 
                xCount += symRevert * (-1);
                curPos += 2;
            }
            if ((curPos[0] == '+') && (curPos[1] == 'x')) { //+x 
                xCount += symRevert * 1;
                curPos += 2;
            }
            if (curPos[0] == 'x') { //x 
                xCount += symRevert * 1;
                curPos += 1;
            }
            if (curPos[0] == '=') { //=
                symRevert = -1;
                curPos += 1;
            }       
        } else { 
            if (nextPos[0] == 'x') { //nx
                xCount += curData;
                curPos = nextPos + 1;
            } else {
                printf("111:sum=%d curData=%d \n", sum, curData);
                sum += curData;
                curPos = nextPos;
            }
        }
    }

    if ((xCount == 0) && (sum != 0)) {
        sprintf(resultBuffer, "%s", "No solution");
    } else if ((xCount == 0) && (sum == 0)) {
        sprintf(resultBuffer, "%s", "Infinite solutions");
    } else {
        xValue = (sum * (-1)) / xCount;
        sprintf(resultBuffer, "x=%lld", xValue);   
    }

    return resultBuffer;
}





atoi strtol 会从首地址开始，直到遇到一个非数字字符，将这些转为整数，可以处理正负数








