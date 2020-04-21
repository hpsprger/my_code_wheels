qsort 的比较函数的两个入参，就是数组中 每个待比较的元素的地址
理论上：内存中的任何数据单元都是可以通过qsort排序的，比如基础数据单元int char等，
又比如 二维数组中的每个一维数组
又比如结构体数组中的每个结构体等等

注意二维数组int a[x][y]  与  一维指针数组int *b[x]在用指针方式表达时，形式都是一样的，都是：int **p
都是两者在内存中的存储形式是完全不同的
int a[x][y]：
int *b[x]:


【注意了：这两个存储方式的 qsort 交换时，交换的内容是不一样的】
下面我们举一个例子，qsort排序二维数组: int *b[x]:

#include<stdio.h>
int data1[2] =  {1,'1'}; //或者malloc 
int data2[2] =  {2,'2'};
int data3[2] =  {3,'3'};
int data4[2] =  {4,'4'};
int data5[2] =  {5,'5'};
int data6[2] =  {6,'6'};

int *data[6] = { 
    data3,
    data1,
    data4,    
    data2,
    data5,
    data6,
};
int compare(const void *a, const void  *b) 
{
    int *arg1= *(int **)a;//数组中每个元素的类型为int *, 数组中每个元素的指针类型为int **
    int *arg2= *(int **)b;//那么数组中每个元素的地址为int **, 也就是元素的指针类型为int **
    if (arg1[0] > arg2[0]) return  1; //用一维数组的0号元素做比较值
    if (arg1[0] < arg2[0]) return -1; //用一维数组的0号元素做比较值
    return 0;
}
int main(void)
{
    int i;
    for (i = 0; i < 6; i++) {
        printf("%c-",data[i][1]);
    }   
    printf("\n");
    qsort(data, 6, sizeof(int *), compare);
    for (i = 0; i < 6; i++) {
        printf("%c-",data[i][1]);
    }   
    printf("\n");
    return 0;
}

hpsp@hpsplnx:~/rock_space/my_code_tst$ ./a.out 
3-1-4-2-5-6-
1-2-3-4-5-6-



qsort排序二维数组: int a[x][y]：


#include<stdio.h>
int data[6][2] = { 
    {2,'2'},
    {1,'1'},
    {5,'5'},
    {3,'3'},
    {4,'4'},
    {0,'0'}
};
int compare(const void *a, const void  *b) 
{
    int *arg1= (int *)a;//转换为 一维数组的起始地址
    int *arg2= (int *)b;//转换为 一维数组的起始地址
    if (arg1[0] > arg2[0]) return  1; //用一维数组的0号元素做比较值
    if (arg1[0] < arg2[0]) return -1; //用一维数组的0号元素做比较值
    return 0;
}
int main(void)
{
    int i;
    for (i = 0; i < 6; i++) {
        printf("%c-",data[i][1]);
    }   
    printf("\n");
    qsort(data, 6, 2*sizeof(int), compare);
    for (i = 0; i < 6; i++) {
        printf("%c-",data[i][1]);
    }   
    printf("\n");
    return 0;
}

hpsp@hpsplnx:~/rock_space/my_code_tst$ ./a.out 
2-1-5-3-4-0-
0-1-2-3-4-5-



下面我们举一个例子，qsort排序结构体数组


#include<stdio.h>

typedef struct Info__ {
    int idx;
    char name[10];
}Info;

Info data[6] = { 
    {2,"two"},
    {1,"one"},
    {5,"five"},
    {3,"three"},
    {4,"four"},
    {0,"zero"}
};

int compare(const void *a, const void  *b) 
{
    Info *arg1= (Info *)a;
    Info *arg2= (Info *)b;

    if (arg1->idx > arg2->idx) return 1;
    if (arg1->idx < arg2->idx) return -1; 
    return 0;
}

int main(void)
{
    int i;
    for (i = 0; i < 6; i++) {
        printf("%s-",data[i].name);
    }
    printf("\n");
    qsort(data, 6, sizeof(Info), compare);
    for (i = 0; i < 6; i++) {
        printf("%s-",data[i].name);
    }
    printf("\n");
    return 0;
}

hpsp@hpsplnx:~/rock_space/my_code_tst$ ./a.out 
two-one-five-three-four-zero-
zero-one-two-three-four-five-


例子1：力扣 4. 寻找两个有序数组的中位数


int compare(const void *a, const void *b) 
{
    int arg1 = *(const int *)a; 
    int arg2 = *(const int *)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;    
}

double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size){
    int *data;
    int size;
    int i;
    int index;

    if (nums1Size == 0) {
        data = nums2;   
        size = nums2Size;     
    } else if (nums2Size == 0) {
        data = nums1;   
        size = nums1Size;     
    } else {
        data = (int *)calloc(nums1Size + nums2Size, sizeof(int));
        size = nums1Size + nums2Size;
        memcpy(data, nums1, sizeof(int) * nums1Size);
        memcpy((data + nums1Size), nums2, sizeof(int) * nums2Size);
        qsort(data, nums1Size + nums2Size, sizeof(int), compare);
    }

    //for (i = 0; i < nums1Size + nums2Size; i++) {
    //        printf("%d ",data[i]);
    //}

    if ((size % 2) == 0) {
        index = (size - 1) / 2;
        return ((double)data[index] + (double)data[index+1]) / 2.0;
    } else {
        index = (size - 1) / 2;
        return (double)data[index];
    }
}


例子2：根据信息  处理父子关系


将每个孩子节点的地址(Node *类型) 存放到父节点的孩子地址的数组中
如果有必要的话，还可以通过qsort根据某个孩子的某个属性值进行排序
孩子排序后，就可以按照一定的顺序对孩子进行逐一的访问了

g_pNodeS[i].childCnt = xxxx;
g_pNodeS[i].childs = (Node **)malloc(sizeof(Node *) * xxxx);

int compare(const void *data1, const void *data2)
{
    Node *pNodeA = *(Node **)data1;//data1 data2 分别是指向数组元素的地址
    Node *pNodeB = *(Node **)data2;//(这里数组元素是Node *类型，元素地址就是Node **类型)

    //printf("pNodeA->s=%d  pNodeB->s=%d \n", pNodeA->s, pNodeB->s);

    if (pNodeA->val < pNodeB->val) return -1;
    if (pNodeA->val > pNodeB->val) return 1;
    return 0;    
}

qsort(g_pNodeS[i].childs, g_pNodeS[i].childCnt, sizeof(Node *), compare);
























