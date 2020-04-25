#include<stdio.h>

typedef struct ListNode_ {
    int data;
    struct ListNode_ *prev;
    struct ListNode_ *next;
}ListNode;

typedef struct List_ {
    unsigned int nodeNums;
    ListNode *listHead;
    ListNode *listTail;
}List;

void PrintListAllNode(List *pList)
{
    ListNode *pNode = 0;

    if (pList == 0) {
        return;
    }

    pNode = pList->listHead;

    while (pNode != 0) {
        printf("data:%d==>", pNode->data);
        pNode = pNode->next;
    }
    printf("\n");
}

ListNode *FindListNode(List *pList, int key)
{
    ListNode *pTmp = 0;
    ListNode *pNode = 0;

    if (pList == 0) {
        return;
    }

    pNode = pList->listHead;

    while (pNode != 0) {
        if (pNode->data == key) {
            pTmp = pNode;
            break;
        }
        pNode = pNode->next;
    }
    return pTmp;
}

void DeleteListNodeByKey(List *pList, int key)
{
    ListNode *pTmp = 0;
    ListNode *pNode = 0;

    if (pList == 0) {
        return;
    }

    pNode = pList->listHead;

    while (pNode != 0) {
        if (pNode->data == key) {
            pTmp = pNode;
            break;
        }
        pNode = pNode->next;
    }

    if (pTmp != 0) {
        if ((pTmp->next == 0) && (pTmp->prev == 0)) { /* 单个节点情况下 */
            pList->listHead = 0;
            pList->listTail = 0;
            free(pTmp);
        } else if ((pTmp->next == 0) && (pTmp->prev != 0)) { /* 多节点时的尾节点 */
            pTmp->prev->next = 0;
            pList->listTail = pTmp->prev;
            free(pTmp);
        } else if ((pTmp->next != 0) && (pTmp->prev == 0)) { /* 多节点时的头节点 */
            pTmp->next->prev = 0;
            pList->listHead = pTmp->next;
            free(pTmp);         
        } else {//其他的情况
            pTmp->prev->next = pTmp->next;
            pTmp->next->prev = pTmp->prev;
            free(pTmp);           
        }
        pList->nodeNums--;
    }
}

/* 这个接口将节点从链表中移除，并释放这个节点的内存  */
void DeleteListNode(List *pList, ListNode *pNode)
{
    ListNode *pTmp = pNode;

    if ((pList == 0) || (pNode == 0)) {
        return;
    }

    if ((pTmp->next == 0) && (pTmp->prev == 0)) { /* 单个节点情况下 */
        printf("1....\n");
        pList->listHead = 0;
        pList->listTail = 0;
        free(pTmp);
    } else if ((pTmp->next == 0) && (pTmp->prev != 0)) { /* 多节点时的尾节点 */
        printf("2....\n");
        pTmp->prev->next = 0;
        pList->listTail = pTmp->prev;
        free(pTmp);
    } else if ((pTmp->next != 0) && (pTmp->prev == 0)) { /* 多节点时的头节点 */
        printf("3....\n");
        pTmp->next->prev = 0;
        pList->listHead = pTmp->next;
        free(pTmp);         
    } else {/* 其他的情况 */
        printf("4....\n");
        pTmp->prev->next = pTmp->next;
        pTmp->next->prev = pTmp->prev;
        free(pTmp);           
    }
    pList->nodeNums--;
}

/* 这个接口只是将节点从链表中移除，没有释放这个节点的内存，释放动作由用户自行释放，反正地址是传入的 */
/* 一个节点删除时，不释放内存，适合用于一个数据被多链表管理的场景 */
void DeleteListNodeWithoutFree(List *pList, ListNode *pNode)
{
    ListNode *pTmp = pNode;

    if ((pList == 0) || (pNode == 0)) {
        return;
    }

    if ((pTmp->next == 0) && (pTmp->prev == 0)) { /* 单个节点情况下 */
        pList->listHead = 0;
        pList->listTail = 0;
    } else if ((pTmp->next == 0) && (pTmp->prev != 0)) { /* 多节点时的尾节点 */
        pTmp->prev->next = 0;
        pList->listTail = pTmp->prev;
    } else if ((pTmp->next != 0) && (pTmp->prev == 0)) { /* 多节点时的头节点 */
        pTmp->next->prev = 0;
        pList->listHead = pTmp->next;       
    } else {/* 其他的情况 */
        pTmp->prev->next = pTmp->next;
        pTmp->next->prev = pTmp->prev;         
    }
    pList->nodeNums--;
}

void DeleteListHead(List *pList)
{
    ListNode *pTmp = 0;

    if (pList == 0) {
        return;
    }

    if (pList->listHead != 0) {
        pTmp = pList->listHead->next;
        free(pList->listHead);
        pList->listHead = pTmp;
        if (pList->listHead != 0) {
            pList->listHead->prev = 0;
        } else {
            pList->listTail = 0;
        }
        pList->nodeNums--;
    }
}

void DeleteListHeadWithoutFree(List *pList)
{
    if (pList == 0) {
        return;
    }

    if (pList->listHead != 0) {
        pList->listHead = pList->listHead->next;
        if (pList->listHead != 0) {
            pList->listHead->prev = 0;
        } else {
            pList->listTail = 0;
        }
        pList->nodeNums--; 
    }
}

void DeleteListTail(List *pList)
{
    ListNode *pTmp = 0;

    if (pList == 0) {
        return;
    }

    if (pList->listTail != 0) {
        pTmp = pList->listTail->prev;
        free(pList->listTail);
        pList->listTail = pTmp;
        if (pList->listTail != 0) {
            pList->listTail->next = 0;
        } else {
            pList->listHead = 0;
        }
        pList->nodeNums--; 
    }
}

void DeleteListTailWithoutFree(List *pList)
{
    if (pList == 0) {
        return;
    }

    if (pList->listTail != 0) {
        pList->listTail = pList->listTail->prev;
        if (pList->listTail != 0) {
            pList->listTail->next = 0;
        } else {
            pList->listHead = 0;
        }
        pList->nodeNums--; 
    }
}

/* O(1) */
void InsertListNodeToHead(List *pList, ListNode *pNode)
{
    if ((pList == 0) || (pNode == 0)) {
        return;
    }

    if (pList->listTail == 0) {
        pList->listTail = pNode;
    }

    if (pList->listHead == 0) {
        pList->listHead = pNode;
    } else {
        pNode->next = pList->listHead;
        pNode->prev = 0;
        pList->listHead->prev = pNode;
        pList->listHead = pNode;
    }
    pList->nodeNums++;
}

/* O(1) */
void InsertListNodeToTail(List *pList, ListNode *pNode)
{
    if ((pList == 0) || (pNode == 0)){
        return;
    }

    if (pList->listHead == 0) {
        pList->listHead = pNode;
    }

    if (pList->listTail == 0) {
        pList->listTail = pNode;
    } else {
        pNode->next = 0;
        pNode->prev = pList->listTail;
        pList->listTail->next = pNode;
        pList->listTail = pNode;
    }
    pList->nodeNums++;    
}

int CompareFunc(const void *a, const void *b)
{
    ListNode * arg1 = *((ListNode **)a);
    ListNode * arg2 = *((ListNode **)b);

    if (arg1->data > arg2->data) return 1;
    if (arg1->data < arg2->data) return -1;
    return 0;
}

void ListSort(List *pList)
{
    unsigned int i;
    unsigned int nodeNums;    
    ListNode *pNode = 0;
    ListNode **pNodeArray = 0;

    if (pList == 0) {
        return;
    }

    if (pList->nodeNums == 0) {
        return;
    }

    pNode = pList->listHead;
    nodeNums = pList->nodeNums;
    pNodeArray = (ListNode **)calloc(nodeNums, sizeof(ListNode *));

    while (pNode != 0) {
        pNodeArray[i++] = pNode;
        pNode = pNode->next;
    }

    for (i = 0; i < nodeNums; i++) {
        printf("11pNodeArray[i]->data:%d==>", pNodeArray[i]->data);
    }

    printf("\n");

    /* 排序没有释放内存，之前的链表关系不用考虑，会重建链表关系 */
    pList->listHead = 0;
    pList->listTail = 0;
    pList->nodeNums = 0;

    qsort(pNodeArray, nodeNums, sizeof(ListNode *), CompareFunc);

    pNodeArray[0]->prev = 0;
    pNodeArray[nodeNums - 1]->next = 0;

    for (i = 0; i < nodeNums; i++) {
        printf("22pNodeArray[i]->data:%d==>", pNodeArray[i]->data);
    }

    printf("\n");

    for (i = 0; i < nodeNums; i++) {
        InsertListNodeToTail(pList, pNodeArray[i]);
    }

    free(pNodeArray);
}

List *CreateList()
{
    List *pList = (List *)calloc(1, sizeof(List));
}

void DestroyList(List *pList)
{
    ListNode *pTmp = 0;
    ListNode *pNode = 0;

    if (pList == 0) {
        return;
    }

    pNode = pList->listHead;
    while (pNode != 0) {
        pTmp = pNode->next;
        free(pNode);
        pNode = pTmp;
    }
    pList->listHead = 0;
    pList->listTail = 0;
    pList->nodeNums = 0;
    free(pList);
}

int main(void)
{
    char order;
    int data;
    ListNode *pNode;
    List *pList = 0;
    while(1){
        printf("\n c:create_list  e:destory_list  d:delete node by key  x:delete node by node f:find_node  h:insert_node_to_head  t:insert_node_to_tail p:print_list s:qsort k:del_list_head l:del_list_tail q:exit \n");
        printf("choose:");
        fflush(stdin);
        scanf("%c",&order);
        if ( order =='q') {
            break;
        } else if (order == 'c') {
            if (pList == 0) {
                pList = CreateList();
                printf("list create ok...0x%p\n", pList);
            } else {
                printf("list exist...\n");
            }
        } else if (order == 'e') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                DestroyList(pList);
                printf("list destroy ok....\n");
                pList = 0;
            }               
        } else if (order == 'h') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                data = 0;
                printf("data=");
                fflush(stdin);
                scanf("%d",&data);
                pNode = 0;
                pNode=(ListNode *)calloc(1, sizeof(ListNode));
                pNode->data = data;
                InsertListNodeToHead(pList, pNode);
                printf("list insert head ok...curNodeNums=%d\n", pList->nodeNums);
            }              
        } else if (order == 't') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                data = 0;
                printf("data=");
                fflush(stdin);
                scanf("%d",&data);
                pNode = 0;
                pNode=(ListNode *)calloc(1, sizeof(ListNode));
                pNode->data = data;
                InsertListNodeToTail(pList, pNode);
                printf("list insert tail ok...curNodeNums=%d\n", pList->nodeNums);
            }  
        } else if (order == 'p') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                PrintListAllNode(pList);
                printf("PrintListAllNode done...curNodeNums=%d\n", pList->nodeNums);
            }
        } else if (order == 'f') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                data = 0;
                printf("key=");
                fflush(stdin);
                scanf("%d",&data);
                pNode = 0;
                pNode = FindListNode(pList, data);
                if (pNode == 0) {
                    printf("key=%d not found...\n", data);
                } else {
                    printf("key=%d found addr=0x%p...\n", data, pNode); 
                }
            }  
        } else if (order == 'd') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                data = 0;
                printf("key=");
                fflush(stdin);
                scanf("%d",&data);
                DeleteListNodeByKey(pList, data);
                printf("DeleteListNodeByKey key=%d curNodeNums=%d\n", data, pList->nodeNums); 
            }
        } else if (order == 's') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                ListSort(pList);
                printf("ListSort done...\n");
            }
        } else if (order == 'x') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                data = 0;
                printf("key=");
                fflush(stdin);
                scanf("%d",&data);
                pNode = FindListNode(pList, data); 
                if (pNode == 0) {
                    printf("key=%d not found...\n", data);
                } else {
                    printf("key=%d found：%p...curNodeNums=%d\n", data, pNode, pList->nodeNums); 
                    DeleteListNode(pList, pNode);
                    printf("DeleteListNode by node delete:%p ...curNodeNums=%d\n", pNode, pList->nodeNums);
                }
            }
        } else if (order == 'k') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                DeleteListHead(pList);
                printf("DeleteListHead ...curNodeNums=%d\n", pList->nodeNums);
            }
        } else if (order == 'l') {
            if (pList == 0) {
                printf("list not exist...\n");
            } else {
                DeleteListTail(pList);
                printf("DeleteListTail ...curNodeNums=%d\n", pList->nodeNums);
            }
        }

        scanf("%c",&order);
    }
    return 0;
}
