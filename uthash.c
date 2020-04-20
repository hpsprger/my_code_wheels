==================================================================================================
sh/src/uthash.h

typedef struct UT_hash_table {
    UT_hash_bucket *buckets;
    unsigned num_buckets, log2_num_buckets;
    unsigned num_items;
    struct UT_hash_handle *tail; /* tail hh in app order, for fast append    */
    ptrdiff_t hho; /* hash handle offset (byte pos of hash handle in element */

    /* in an ideal situation (all buckets used equally), no bucket would have
     *     * more than ceil(#items/#buckets) items. that's the ideal chain length. */
    unsigned ideal_chain_maxlen;

    /* nonideal_items is the number of items in the hash whose chain position
     *     * exceeds the ideal chain maxlen. these items pay the penalty for an uneven
     *         * hash distribution; reaching them in a chain traversal takes >ideal steps */
    unsigned nonideal_items;

    /* ineffective expands occur when a bucket doubling was performed, but
     *     * afterward, more than half the items in the hash had nonideal chain
     *         * positions. If this happens on two consecutive expansions we inhibit any
     *             * further expansion, as it's not helping; this happens when the hash
     *                 * function isn't a good fit for the key domain. When expansion is inhibited
     *                     * the hash will still work, albeit no longer in constant time. */
    unsigned ineff_expands, noexpand;

    uint32_t signature; /* used only to find hash tables in external analysis */
#ifdef HASH_BLOOM
    uint32_t bloom_sig; /* used only to test bloom exists in external analysis */
    uint8_t *bloom_bv;
    uint8_t bloom_nbits;
#endif

} UT_hash_table;

typedef struct UT_hash_handle {
    struct UT_hash_table *tbl;
    void *prev;                       /* prev element in app order      */
    void *next;                       /* next element in app order      */
    struct UT_hash_handle *hh_prev;   /* previous hh in bucket order    */
    struct UT_hash_handle *hh_next;   /* next hh in bucket order        */
    void *key;                        /* ptr to enclosing struct's key  */
    unsigned keylen;                  /* enclosing struct's key len     */
    unsigned hashv;                   /* result of hash-fcn(key)        */
} UT_hash_handle;

========================================================================================


#include <stdlib.h>  
#include <stdio.h>  
#include "./uthash/src/uthash.h"

/* one key include multi sub_data */
typedef struct { 
    char a;  
    int  b;  
} HashKey;  

typedef struct { 
    int data;  
} HashValue;

typedef struct { 
    HashKey        key;
    HashValue      value;
    UT_hash_handle hh;  
} HashEntry;

int sort_func(HashEntry *a, HashEntry *b) 
{  
    if (a->key.b > b->key.b) return 1;
    if (a->key.b < b->key.b) return -1; 
    return 0;  
}

int main(int argc, char *argv[]) 
{ 
    HashKey   findKey; 
    HashEntry *pResult; 
    HashEntry *pTmp; 
    HashEntry *pCurEntry; 
    HashEntry *pEntry1; 
    HashEntry *pEntry2; 
    HashEntry *pEntry3; 
    HashEntry *pEntry4;
    HashEntry *pHashTable = NULL;/*hash table pointer*/  
    int count;

    pEntry1 = (HashEntry*)malloc(sizeof(HashEntry));  
    memset(pEntry1, 0, sizeof(HashEntry)); 
    pEntry1->key.a = 'a';  
    pEntry1->key.b = 5;  
    pEntry1->value.data=0x1234;
    HASH_ADD(hh, pHashTable, key, sizeof(HashKey), pEntry1);  

    pEntry2 = (HashEntry*)malloc(sizeof(HashEntry));  
    memset(pEntry2, 0, sizeof(HashEntry)); 
    pEntry2->key.a = 'b';  
    pEntry2->key.b = 2;  
    pEntry2->value.data=0x5678;
    HASH_ADD(hh, pHashTable, key, sizeof(HashKey), pEntry2);

    pEntry3 = (HashEntry*)malloc(sizeof(HashEntry));
    memset(pEntry3, 0, sizeof(HashEntry));
    pEntry3->key.a = 'b';
    pEntry3->key.b = 2;
    pEntry3->value.data=0x9abc;
    HASH_ADD(hh, pHashTable, key, sizeof(HashKey), pEntry3);

    pEntry4 = (HashEntry*)malloc(sizeof(HashEntry));
    memset(pEntry4, 0, sizeof(HashEntry));
    pEntry4->key.a = 'c';
    pEntry4->key.b = 1;
    pEntry4->value.data=0x2345;
    HASH_ADD(hh, pHashTable, key, sizeof(HashKey), pEntry4);

    memset(&findKey, 0, sizeof(HashKey));
    findKey.a = 'a';
    findKey.b = 1;
    HASH_FIND(hh, pHashTable, &findKey, sizeof(HashKey), pResult);
    if (pResult) {
        printf("found entry: key.a=%c key.b=%d entry_value=0x%x\n", pResult->key.a, pResult->key.b, pResult->value.data);
    } else {
        printf("not found entry: findKey.a=%c findKey.b=%d \n", findKey.a, findKey.b);
    }

    memset(&findKey, 0, sizeof(HashKey));
    findKey.a = 'b';
    findKey.b = 2;
    HASH_FIND(hh, pHashTable, &findKey, sizeof(HashKey), pResult);
    while (pResult) {
        if (pResult) {
            printf("found entry: key.a=%c key.b=%d entry_value=0x%x\n", pResult->key.a, pResult->key.b, pResult->value.data);
        } else {
            printf("not found entry: findKey.a=%c findKey.b=%d \n", findKey.a, findKey.b);
        }
        pResult = pResult->hh.next;
        printf("pResult = %p \n", pResult);
    }

    count = HASH_COUNT(pHashTable);
    printf("there are %d entrys \n", count);
    for (pTmp = pHashTable; pTmp != NULL; pTmp=pTmp->hh.next) {
        printf("entry: key.a=%c: key.b=%d value.data=0x%x \n", pTmp->key.a, pTmp->key.b, pTmp->value.data);
    }

    printf("sort...\n");
    HASH_SORT(pHashTable, sort_func);
    for (pTmp = pHashTable; pTmp != NULL; pTmp=pTmp->hh.next) {
        printf("entry: key.a=%c: key.b=%d value.data=0x%x \n", pTmp->key.a, pTmp->key.b, pTmp->value.data);
    }

    printf("delete entry3....\n");
    pCurEntry = pEntry3;
    HASH_DEL(pHashTable, pCurEntry);
    free(pCurEntry);【见下面删除头ENTRY的说明，删除的时候一定要注意：要用一个中间体来保存待删除的地址，尤其是删除头ENTRY的时候，很容易出错】

        memset(&findKey, 0, sizeof(HashKey));
    findKey.a = 'b';
    findKey.b = 2;
    HASH_FIND(hh, pHashTable, &findKey, sizeof(HashKey), pResult);
    if (pResult) {
        printf("found entry: key.a=%c key.b=%d entry_value=0x%x\n", pResult->key.a, pResult->key.b, pResult->value.data);
    } else {
        printf("not found entry: findKey.a=%c findKey.b=%d \n", findKey.a, findKey.b);
    }

    count = HASH_COUNT(pHashTable);
    printf("there are %d entrys \n", count);
    for (pTmp = pHashTable; pTmp != NULL; pTmp=pTmp->hh.next) {
        printf("entry: key.a=%c: key.b=%d value.data=0x%x \n", pTmp->key.a, pTmp->key.b, pTmp->value.data);
    }

    printf("delete first entry....\n");
    pCurEntry = pHashTable;
    HASH_DEL(pHashTable, pCurEntry);
    free(pCurEntry); 【这里不能直接HASH_DEL(pHashTable, pHashTable); free(pHashTable);
    HASH_DEL后头结点已更新，这样做会导致后面的free是free了新的节点内存
        所以删除的时候一定要注意：要用一个中间体来保存待删除的地址，尤其是删除头ENTRY的时候，很容易出错】

        count = HASH_COUNT(pHashTable);
    printf("there are %d entrys \n", count);
    for (pTmp = pHashTable; pTmp != NULL; pTmp=pTmp->hh.next) {
        printf("entry: key.a=%c: key.b=%d value.data=0x%x \n", pTmp->key.a, pTmp->key.b, pTmp->value.data);
    }

    printf("free all entry....\n");
    HASH_ITER(hh, pHashTable, pCurEntry, pTmp) {
        printf("delete 0x%p from hashtable..\n", pCurEntry);
        HASH_DEL(pHashTable, pCurEntry);  /* delete it (users advances to next) */
        printf("free mem... \n");
        free(pCurEntry);                  /* free it */
    }

    return 0;
}



hpsp@hpsplnx:~/rock_space/uthash$ ./a.out 
    not found entry: findKey.a=a findKey.b=1 
    found entry: key.a=b key.b=2 entry_value=0x9abc
    pResult = 0x55711d3485b0 
    found entry: key.a=c key.b=1 entry_value=0x2345
    pResult = (nil) 
    there are 4 entrys 
    entry: key.a=a: key.b=5 value.data=0x1234 
    entry: key.a=b: key.b=2 value.data=0x5678 
    entry: key.a=b: key.b=2 value.data=0x9abc 
    entry: key.a=c: key.b=1 value.data=0x2345 
    sort...
    entry: key.a=c: key.b=1 value.data=0x2345 
    entry: key.a=b: key.b=2 value.data=0x5678 
    entry: key.a=b: key.b=2 value.data=0x9abc 
    entry: key.a=a: key.b=5 value.data=0x1234 
    delete entry3.... 【有两个同KEY的】
    found entry: key.a=b key.b=2 entry_value=0x5678
    there are 3 entrys 
    entry: key.a=c: key.b=1 value.data=0x2345 
    entry: key.a=b: key.b=2 value.data=0x5678 
    entry: key.a=a: key.b=5 value.data=0x1234 
    delete first entry....
    there are 2 entrys 
    entry: key.a=b: key.b=2 value.data=0x5678 
    entry: key.a=a: key.b=5 value.data=0x1234 
    free all entry....
    delete 0x0x55711d348510 from hashtable..
    free mem... 
    delete 0x0x55711d348260 from hashtable..
    free mem... 

=============================================================

leetcode 146


运用你所掌握的数据结构，设计和实现一个  LRU (最近最少使用) 缓存机制。它应该支持以下操作： 获取数据 get 和 写入数据 put 。

获取数据 get(key) - 如果密钥 (key) 存在于缓存中，则获取密钥的值（总是正数），否则返回 -1。
写入数据 put(key, value) - 如果密钥已经存在，则变更其数据值；如果密钥不存在，则插入该组「密钥/数据值」。当缓存容量达到上限时，它应该在写入新数据之前删除最久未使用的数据值，从而为新的数据值留出空间。
进阶:
你是否可以在 O(1) 时间复杂度内完成这两种操作？
示例:
LRUCache cache = new LRUCache( 2 /* 缓存容量 */ );

cache.put(1, 1);
cache.put(2, 2);
cache.get(1);       // 返回  1
cache.put(3, 3);    // 该操作会使得密钥 2 作废
cache.get(2);       // 返回 -1 (未找到)
cache.put(4, 4);    // 该操作会使得密钥 1 作废
cache.get(1);       // 返回 -1 (未找到)
cache.get(3);       // 返回  3
cache.get(4);       // 返回  4

来源：力扣（LeetCode）
链接：https://leetcode-cn.com/problems/lru-cache
著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

["LRUCache","get","put","get","put","put","get","get"]
[[2],[2],[2,6],[1],[1,5],[1,2],[1],[2]]

这个解法引入了HASH_SORT，力扣上面导致有些大用例超时了，所以见下面的解法，去掉了HASH_SORT，利用链表的节点插入特性
加入我们定义的HashEntry *hashTable 是链表头部，
实际测试发现:
HASH_ADD的时候，新插入的节点都是默认插入在链表尾部，这样我们定义的HashEntry *hashTable 永远是最老的节点
如果中间某个节点被访问了，这时的做法是把这个节点先从HASH TABLE中删除，然后再重新插入，这样保证节点是最新的
实测发现，这个方法可以解决力扣用例超时的问题，见后面的代码

=====================长用例超时代码：HASH_SORT=============================
#include <uthash.h>

typedef struct HashEntry_ {
    int key;
    int value;
    unsigned int idx;
    UT_hash_handle hh;
}HashEntry;

typedef struct {
    HashEntry *hashTable;
    int capacity;
    int usedCnt;
    unsigned int putCnt;
}LRUCache;

LRUCache* lRUCacheCreate(int capacity) {
    LRUCache *pTmp = (LRUCache *)calloc(1, sizeof(LRUCache));
    pTmp->capacity = capacity;
    return pTmp;
}

int SortFunc(HashEntry *a, HashEntry *b) 
{
    if (a->idx > b->idx) return 1;
    if (a->idx < b->idx) return -1;
    return 0;
}

int lRUCacheGet(LRUCache* obj, int key) {
    HashEntry *pEntry = 0;
    if (obj->hashTable == 0) {
        return -1;
    }
    HASH_FIND(hh, obj->hashTable, &key, sizeof(int), pEntry);
    if (pEntry == 0) {
        return -1;
    } else {
        pEntry->idx = obj->putCnt++;
        HASH_SORT(obj->hashTable, SortFunc);
        return pEntry->value;
    }
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    HashEntry *pResult = 0;
    HashEntry *pTmp = 0;
    HashEntry *pEntry = 0;

    HASH_FIND(hh, obj->hashTable, &key, sizeof(int), pResult);
    if (pResult != 0) {
        HASH_DEL(obj->hashTable, pResult);
        obj->usedCnt--;
        free(pResult);
    }

    if (obj->usedCnt == obj->capacity) {
        pTmp = obj->hashTable;
        HASH_DEL(obj->hashTable, pTmp);//首节点排序后为最老的节点
        obj->usedCnt--;
        free(pTmp);
    }

    pEntry = (HashEntry *)calloc(1, sizeof(HashEntry));
    pEntry->key = key;
    pEntry->value = value;
    pEntry->idx = obj->putCnt++;
    HASH_ADD(hh, obj->hashTable, key, sizeof(int), pEntry);
    obj->usedCnt++;
    HASH_SORT(obj->hashTable, SortFunc);
}

void lRUCacheFree(LRUCache* obj) {
    HashEntry *pCurEntry = 0;
    HashEntry *pTmp = 0;
    HASH_ITER(hh, obj->hashTable, pCurEntry, pTmp) {
        HASH_DEL(obj->hashTable, pCurEntry);  /* delete it (users advances to next) */
        free(pCurEntry);                  /* free it */
    }
}

=====利用ENTRY插入位置的特性，自然解决最老的排序问题，解决长用例超时问题：未用HASH_SORT========

#include <uthash.h>

typedef struct HashEntry_ {
    int key;
    int value;
    unsigned int idx;
    UT_hash_handle hh;
}HashEntry;

typedef struct {
    HashEntry *hashTable;
    int capacity;
    int usedCnt;
    unsigned int putCnt;
}LRUCache;

LRUCache* lRUCacheCreate(int capacity) {
    LRUCache *pTmp = (LRUCache *)calloc(1, sizeof(LRUCache));
    pTmp->capacity = capacity;
    return pTmp;
}

int SortFunc(HashEntry *a, HashEntry *b) 
{
    if (a->idx > b->idx) return 1;
    if (a->idx < b->idx) return -1;
    return 0;
}

int lRUCacheGet(LRUCache* obj, int key) {
    HashEntry *pEntry = 0;
    if (obj->hashTable == 0) {
        return -1;
    }
    HASH_FIND(hh , obj->hashTable, &key, sizeof(int), pEntry);
    if (pEntry == 0) {
        return -1;
    } else {
        //HASH_SORT(obj->hashTable, SortFunc); //排序太耗时了，导致最终执行结果超时
        HASH_DEL(obj->hashTable, pEntry);
        HASH_ADD(hh, obj->hashTable, key, sizeof(int), pEntry);//添加时总是将最新的添加到链表尾部，这样能保证链表头部是最早插入的
        return pEntry->value;
    }
}

void lRUCachePut(LRUCache* obj, int key, int value) {
    HashEntry *pResult = 0;
    HashEntry *pTmp = 0;
    HashEntry *pEntry = 0;

    HASH_FIND(hh, obj->hashTable, &key, sizeof(int), pResult);
    if (pResult != 0) {
        HASH_DEL(obj->hashTable, pResult);
        obj->usedCnt--;
        free(pResult);
    }

    if (obj->usedCnt == obj->capacity) {
        pTmp = obj->hashTable;
        HASH_DEL(obj->hashTable, pTmp);//首节点排序后为最老的节点
        obj->usedCnt--;
        free(pTmp);
    }

    pEntry = (HashEntry *)calloc(1, sizeof(HashEntry));
    pEntry->key = key;
    pEntry->value = value;
    HASH_ADD(hh, obj->hashTable, key, sizeof(int), pEntry);
    obj->usedCnt++;
    //HASH_SORT(obj->hashTable, SortFunc);
}

void lRUCacheFree(LRUCache* obj) {
    HashEntry *pCurEntry = 0;
    HashEntry *pTmp = 0;
    HASH_ITER(hh, obj->hashTable, pCurEntry, pTmp) {
        HASH_DEL(obj->hashTable, pCurEntry);  /* delete it (users advances to next) */
        free(pCurEntry);                  /* free it */
    }
}



leetcode 582 
===================================================================


给 n 个进程，每个进程都有一个独一无二的 PID （进程编号）和它的 PPID （父进程编号）。

每一个进程只有一个父进程，但是每个进程可能会有一个或者多个孩子进程。它们形成的关系就像一个树状结构。只有一个进程的 PPID 是 0 ，意味着这个进程没有父进程。所有的 PID 都会是唯一的正整数。

我们用两个序列来表示这些进程，第一个序列包含所有进程的 PID ，第二个序列包含所有进程对应的 PPID。

现在给定这两个序列和一个 PID 表示你要杀死的进程，函数返回一个 PID 序列，表示因为杀这个进程而导致的所有被杀掉的进程的编号。

当一个进程被杀掉的时候，它所有的孩子进程和后代进程都要被杀掉。

你可以以任意顺序排列返回的 PID 序列。

示例 1:

输入: 
pid =  [1, 3, 10, 5]
ppid = [3, 0, 5, 3]
kill = 5
输出: [5,10]

#include <uthash.h>

typedef struct {
    int key;
    int value;
    UT_hash_handle hh;
}HashTableEntry;

typedef struct {
    int idx;
    int data[100000];
}Stack;

void PushStack(Stack *pStack, int data)
{
    pStack->data[pStack->idx++] = data;
}

int PopStack(Stack *pStack)
{
    if (pStack->idx == 0) {
        return -1;
    }
    return pStack->data[--pStack->idx];
}

int IsStackEmpty(Stack *pStack)
{
        return (pStack->idx == 0) ? 1 : 0;
}


int* killProcess(int* pid, int pidSize, int* ppid, int ppidSize, int kill, int* returnSize){
    int killPid = kill;
    HashTableEntry *pTmp = 0;
    HashTableEntry *pFindRslt = 0;
    HashTableEntry *pHeadEntry = 0;
    HashTableEntry *pCurEntry = 0;    
    int i,j;
    int killCnt = 0;
    int *pRslt = (int *)calloc(2*ppidSize, sizeof(int));
    Stack *pStack = (Stack *)calloc(1, sizeof(Stack));

    printf("0000=%d \n", killPid);

    if (ppidSize == 0) {
        *returnSize = 0;
        return 0;
    }

#if 1
    for (i = 0; i < ppidSize; i++) {
        printf("11111\n");
        pTmp = (HashTableEntry *)calloc(1, sizeof(HashTableEntry));
        printf("2222\n");
        pTmp->key   = ppid[i];
        pTmp->value = pid[i];
        printf("3333\n");
        HASH_ADD(hh, pHeadEntry, key, sizeof(int), pTmp);//HASH_ADD 是每次插入到定义的头ENTRY 所在链表的尾部
        printf("4444\n");
    }
#endif
    printf("5555\n");
    pRslt[killCnt++] = killPid;
    do {
        printf("6666 =%d \n", killPid);
        HASH_FIND(hh , pHeadEntry, &killPid, sizeof(int), pFindRslt);
        if (pFindRslt != 0) { //一个父进程可能有多个子进程 多次FIND，找到一个删掉一个
            printf("7777 =%d \n", killPid);
            PushStack(pStack, pFindRslt->value);
            HASH_DEL(pHeadEntry, pFindRslt);
            free(pFindRslt);
        } else {
            printf("8888 =%d \n", killPid);
            if (IsStackEmpty(pStack) == 0) {
                killPid = PopStack(pStack);
                pRslt[killCnt++] = killPid;
            } else {
                break;
            }
        }
    } while(1);

    HASH_ITER(hh, pHeadEntry, pCurEntry, pTmp) {
        HASH_DEL(pHeadEntry, pCurEntry);  /* delete it (users advances to next) */
        free(pCurEntry);                  /* free it */
    }

    *returnSize = killCnt;
    return pRslt;
}













