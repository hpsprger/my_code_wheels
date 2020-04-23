strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串。
如果是，则该函数返回 str1字符串从 str2第一次出现的位置开始到 str1结尾的字符串；否则，返回NULL。

strstr(string，search，before_search)
    string    必需。规定被搜索的字符串。
    search    必需。规定所搜索的字符串。如果此参数是数字，则搜索匹配此数字对应的 ASCII 值的字符。
    before_search    可选。默认值为 "false" 的布尔值。如果设置为 "true"，它将返回 search 参数第一次出现之前的字符串部分。


strchr函数功能为在一个串中查找给定字符的第一个匹配之处。
函数原型为：char *strchr(const char *str, int c)，即在参数 str 所指向的字符串中搜索第一次出现字符 c（一个无符号字符）的位置。
strchr函数包含在C 标准库 <string.h>

strrchr() 函数（在php中）查找字符在指定字符串中从右面开始的第一次出现的位置，如果成功，返回该字符以及其后面的字符，如果失败，则返回 NULL。与之相对应的是strchr()函数，它查找字符串中首次出现指定字符以及其后面的字符。


