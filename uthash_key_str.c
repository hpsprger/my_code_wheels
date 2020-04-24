#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include"./uthash/src/uthash.h"

/* one key include multi sub_data */
#define NAME_MAX_LEN (50)
typedef struct { 
    char name[NAME_MAX_LEN];
} HashKey;

typedef struct { 
    int data;
} HashValue;

typedef struct { 
    HashKey        key;
    HashValue      value;
    UT_hash_handle hh;
} HashEntry;

int sort_compare_value_func(HashEntry *a, HashEntry *b) 
{   
    if (a->value.data > b->value.data) return 1;
    if (a->value.data < b->value.data) return -1;
    return 0;
}
#if 0 
int main(int argc, char *argv[]) 
{ 
    char name[NAME_MAX_LEN] = {0};
    HashKey   findKey; 
    HashEntry *pResult; 
    HashEntry *pTmp; 
    HashEntry *pCurEntry; 
    HashEntry *pEntry; 
    HashEntry *pHashTableEntryHead = NULL;/*hash table pointer must !!!! */  
    char order;
    int data;

    while (1) {
        printf("\n a:add_entry e:free_all_entry  f:find_entry  d:del_entry s:sort p:print_add_entry q:exit \n");
        printf("choose:");
        fflush(stdin);
        scanf("%c",&order);
        if ( order =='q') {
            break;
        } else if (order == 'a') {
            /* 
             *                 key值相同，重复插入时，是不会替换老的entry
             *                                 a:add_entry e:free_all_entry  f:find_entry  d:del_entry s:sort p:print_add_entry q:exit 
             *                                                 choose:p
             *                                                                 entry: key.a=1 value.data=2 
             *                                                                                 entry: key.a=1 value.data=3 
             *                                                                                                 entry: key.a=1 value.data=5 
             *                                                                                                                 entry: key.a=1 value.data=4 
             *                                                                                                                                 entry: key.a=5 value.data=6 
             *                                                                                                                                                 entry: key.a=6 value.data=12  ==> 最晚插入的ENTRY，find时找到的是这个ENTRY 
             *
             *                                                                                                                                                                 entry都被管理在一个entry_list链表中：
             *                                                                                                                                                                                 pHashTableEntryHead 就是这个链表头ENTRY
             *                                                                                                                                                                                                 按照HASH_ADD加入到entry_list的尾部，默认按照HASH_ADD的顺序，idx:0-->+1 ==> pHashTableEntryHead:entry0(oldest 最老插入的节点)-->entry1-->entry2-->entry3(newest 最新插入的节点)--> 
             *                                                                                                                                                                                                                 然后经过哈希函数映射:key ---哈希算法---> entry_address 快速找到entry的地址
             *                                                                                                                                                                                                                                 如果key有冲突 或者 key重复插入, key对应到一个目的entry的链表: entry_address1(最新的目的entry) --> entry_address0-->....最新的目的entry...
             *                                                                                                                                                                                                                                                                               这时通过key找到的entry就是这个目的entry的链表的头节点，也就是最新插入的这个entry
             *                                                                                                                                                                                                                                                                                                                             且所有的entry 都是 在 entry_list链表中的
             *                                                                                                                                                                                                                                                                                                                                             我们通过对这个entry_list排序，可以使得你想要的东西排队到头节点pHashTableEntryHead去，方便一些 排序删除操作等 
             *                                                                                                                                                                                                                                                                                                                                                                                          比如我们通过时间排序，将最老的节点放在头部，然后只要删除entry_list头部节点就可以删除目前最老的节点，其实这个时间特性，按照默认的HASH_ADD的顺序就可以达到最老的时间排序
             *                                                                                                                                                                                                                                                                                                                                                                                                                                       pHashTableEntryHead:entry1-->entry3-->entry0-->entry2--> 
             *                                                                                                                                                                                                                                                                                                                                                                                                                                                       排序不影响key与entry 之间的哈希映射
             *                                                                                                                                                                                                                                                                                                                                                                                                                                                                       插入 删除节点 会更新 key与entry 之间的哈希映射链表
             *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   */
            pEntry = (HashEntry*)calloc(1,  sizeof(HashEntry));
            data = 0;
            printf("name=");
            fflush(stdin);
            scanf("%s", name);
            strcpy(pEntry->key.name, name);
            printf("key_name=%s\n", pEntry->key.name);
            printf("value=");
            fflush(stdin);
            scanf("%d", &data);
            pEntry->value.data = data;
            HASH_ADD(hh, pHashTableEntryHead, key, sizeof(HashKey), pEntry);
            printf("add entry done...entry_nums=%d", HASH_COUNT(pHashTableEntryHead));
        } else if (order == 'e') {
            printf("free all entry....\n");
            HASH_ITER(hh, pHashTableEntryHead, pCurEntry, pTmp) {
                printf("delete 0x%p from hashtable..\n", pCurEntry);
                HASH_DEL(pHashTableEntryHead, pCurEntry);  /* delete it (users advances to next) */
                printf("free mem... \n");
                free(pCurEntry);                  /* free it */
            }
        } else if (order == 'f') {
            data = 0;
            memset(&findKey, 0, sizeof(HashKey));
            printf("name=");
            fflush(stdin);
            scanf("%s", name);
            strcpy(findKey.name, name);
            printf("findKey_name=%s\n", findKey.name);
            HASH_FIND(hh, pHashTableEntryHead, &findKey, sizeof(HashKey), pResult);
            if (pResult) {
                printf("found entry: key.name=%s value=0x%x...\n", pResult->key.name, pResult->value.data);
            } else {
                printf("findKey.name=%s not found...\n", findKey.name);
            }
        } else if (order == 'd') {   
            printf("delete entry....\n");
            memset(&findKey, 0, sizeof(HashKey));
            printf("name=");
            fflush(stdin);
            scanf("%s", name);
            strcpy(findKey.name, name);
            printf("findKey_name=%s\n", findKey.name);
            HASH_FIND(hh, pHashTableEntryHead, &findKey, sizeof(HashKey), pResult);
            if (pResult) {
                printf("found entry: key.name=%s value=0x%x...\n", pResult->key.name, pResult->value.data);
                HASH_DEL(pHashTableEntryHead, pResult);
                free(pResult);
            } else {
                printf("findKey.name=%s not found...\n", findKey.name);
            }
        } else if (order == 's') {
            printf("key sort...\n");
            HASH_SORT(pHashTableEntryHead, sort_compare_value_func);
        } else if (order == 'h') {
            printf("delete entry_list head ...\n");
            pTmp = pHashTableEntryHead;
            HASH_DEL(pHashTableEntryHead, pTmp);
            free(pTmp);
        } else if (order == 'p') {
            for (pTmp = pHashTableEntryHead; pTmp != NULL; pTmp=pTmp->hh.next) {
                printf("entry: key.data=%s value.data=%d \n", pTmp->key.name, pTmp->value.data);
            }
        }
        scanf("%c",&order);
    }

    return 0;
}
#endif


