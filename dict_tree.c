#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int charmapping[256]; //字符映射数组,charmapping[i]=x表示ascii码为i的字符对应于treenode中的next[x]

#define MAXN (26)
const int maxn=26; //这里假设字符串中只出现26个小写字母
const int maxm=100000;

typedef struct treenode_{
    int count; //标志此节点所表示字符串在所有字符串中以前缀形式出现的总次数
    struct treenode_ *next[MAXN];
}treenode;

treenode  head; 

void init_charmapping()
{
    int i;
    for (i = 'a'; i <= 'z'; i++) { //我的这个字典树现在只允许输入小写字符组成的字符串,然而由于有charmapping的存在,增加新字符添加映射并且增大maxn就好,很方便.
        charmapping[i] = i - 'a';
    }
}

void init_trie()
{
    int i;
    head.count = 1; //初始化为1包括空串并且避免树头被删
    for(i=0; i < maxn; i++) {
        head.next[i] = NULL;
    }
}

treenode* create_newnode()
{ //申请一个新结点并初始化它
    int i;
    treenode *newnode;
    newnode = (treenode *)malloc(sizeof(treenode));
    newnode->count = 0;
    for(i=0; i < maxn; i++) {
        newnode->next[i] = NULL;
    }
    return newnode;
}

/* 向字典树添加长度为num的字符串s */
void update(char *s, int num)
{     
    int k = 0, temp;
    treenode *t = &head;
    while(s[k] != '\0') {
        t->count += num;
        temp = charmapping[s[k]];
        if (t->next[temp] == NULL) {
            t->next[temp] = create_newnode();
        }
        t = t->next[temp];
        k++;
    }
    t->count += num;
}

/* 查找字典树中是否已经存在长度为num的字符串s */
bool search(char* s, int num)
{   
    int i, k = 0, temp;
    int snum;
    treenode *t = &head;

    while (s[k] != '\0') {
        temp = charmapping[s[k]];
        if((t->next[temp] == NULL) || (t->next[temp]->count < num)) {
            return false; //根本不存在字符串s或者存在的数目小于num直接失败
        }
        t = t->next[temp];
        k++;
    }

    snum = t->count;
    for (i = 0; i < maxn; i++) {
        if (t->next[i] != NULL) {
            snum -= t->next[i]->count; //这里是核心!!!结点t代表的字符串出现的次数就是总次数减去所有子节点次数和
        }
    }
    if (snum >= num) {
        return true; //如果字符串s的数目snum大于等于num
    }
    return false;
}

void erase(char* s,int num)
{  //删除字典树中的num个字符串s并释放无用结点,删除前一定要先search是否存在
    int k=0,temp;
    treenode* t=&head;
    treenode* t1; //t1后面的结点都是删除后需要被释放的
    head.count-=num;
    while(s[k]){
        temp=charmapping[s[k]];
        t->next[temp]->count-=num;
        if(t->next[temp]->count==0){
            t1=t->next[temp];
            t->next[temp]=NULL;
            k++;
            break;
        }
        t=t->next[temp];
        k++;
    }
    while(s[k]){ //释放无用结点 
        temp=charmapping[s[k]];
        t=t1->next[temp];
        free(t1);
        t1=t;
        k++;
    }
    free(t1);
}

char temp[1000];
void printall(treenode* tnode,int pos)
{ //递归打印字典树咯,打出了就是字典序升序的 
    int i;
    int count=tnode->count;
    for(i=0;i<maxn;i++) if(tnode->next[i]) count-=tnode->next[i]->count;
    for(i=0;i<count;i++) printf("\"%s\"\n",temp);
    for(i='a';i<='z';i++){
        if(tnode->next[charmapping[i]]){
            temp[pos]=i;
            temp[++pos]='\0';
            printall(tnode->next[charmapping[i]],pos);
            temp[--pos]='\0';
        }
    }
}

int main()
{
    init_charmapping(); //初始化映射
    init_trie();        //初始化字典树
    char x[1000];
    char order; //命令
    int num;    //数目
    printf("q：查询\nu：插入\nd：删除\np：打印字典树\ne：退出\n");
    while(1){
        printf("请输入命令：");
        fflush(stdin);
        scanf("%c",&order);
        if(order=='q'){
            printf("请输入要查找的字符串与数目：");
            scanf("%s%d",&x,&num);
            if(search(x,num)) printf("匹配成功。\n\n");
            else printf("匹配失败，不存在%d个\"%s\"\n\n",num,x); 
        }
        else if(order=='u'){
            printf("请输入要插入的字符串与数目：");
            scanf("%s%d",&x,&num);
            update(x,num);
            printf("%d个\"%s\"已加入字典树。\n\n",num,x);
        }
        else if(order=='d'){
            printf("请输入要删除的字符串与数目：");
            scanf("%s%d",&x,&num);
            if(!search(x,num)){
                printf("树中无%d个字符串\"%s\"请重新键入命令！\n\n",num,x);
                continue;
            }
            erase(x,num);
            printf("%d个\"%s\"已从字典树中删除。\n\n",num,x);
        }
        else if(order=='p'){
            printf("当前字典树内有如下字符串：\n");
            temp[0]='\0';
            printall(&head,0);
        }
        else if(order=='e'){
            printf("退出ing....\n");
            break;
        }
        else printf("无效命令,请重新输入！\n命令q：查询是否存在字符串\n命令u：往字典树加入字符串\n命令d：删除某个字符串\n命令p：按字典序升序输出字典树\n命令e：退出程序\n\n");
    }
    return 0;
}


