
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//include/linux/types.h
//include/linux/list.h

//PS
//we access a list by list_head always 
//list_head is Embedded in a struct with date, the strcuct form a list by list_head
//if we konw a list_node's address,we can use the list_node travers the whole list,so the list_node is the list head
//in linux kernel,when we know a struct address,we can use LIST_ENTRY( container_of) to get the list_node's address 
//so in linux kernel,all list_node call list_head

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

//the macro can get the start_address of the struct(type) including the member
//ptr is the address of the struct's member   
//type is the type of the struct
//member is a member name in struct(type), and the member's address is ptr
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})


struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

//step1: we define a list_head instance by the macro 
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)


//the macro can get the start_address of the struct(type) including the member
//ref the comment of the container_of 
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

//ref the comment of the container_of and list_entry
//pos is address of the struct including a list_node
//head is address of INIT_HEAD node that we use LIST_HEAD(my_list),my_list is the INIT_HEAD node
//member is a member name in struct(typeof(*pos))
//we can use the macro access a struct_list which formed by head_list
//we access the each struct node by corresponding head_node
//we traverse a struct list (if we know list_node's address, list node is list head)
//1.we know the one struct node address(pos) 
//2.we know the struct_node's member(head_node(headlist)) address
//3.we know the name of the struct_node's member(head_node(headlist))
//4.we can use list_for_each_entry(struct_node_address_pos, struct_node_listhead_member_address_head, struct_node_listhead_member_name) access all node of the struct list 
//here:head of the input param of list_for_each_entry donot need be accessed,because wo have known the address of the structm, so we can access it derectly, There is no need to access it through traversal
//here:generally head of the input param of list_for_each_entry  is the list_head that we init list by LIST_HEAD, so this head node just a list node without responding struct,so when  we travers list, wo donot access the list_head's responding struct by Conditional judgement
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member); &pos->member != (head); pos = list_entry(pos->member.next, typeof(*pos), member))

//what is the purpose of the function: if we found the node you desired, you will delet it and free it ,so when you free it, if you donot use list_for_each_entry, segment fault will occur, because th pos mem if invalid
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member), n = list_entry(pos->member.next, typeof(*pos), member);&pos->member != (head); \
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

//prev is the _new_node's prev
//next is the _new_node's next
static inline void __list_add(struct list_head *_new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}



//prev is the _del_node's prev
//next is the _del_node's next
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = (struct list_head*)LIST_POISON1;
	entry->prev = (struct list_head*)LIST_POISON2;
}


/***********************test code**************************************/

typedef struct _packet {
    char name[50];
    unsigned int index;
    struct list_head list_head;
}packet;

static unsigned int packet_index = 1;

int main(void)
{
    int choice;
    int index;
    packet *pPkt = NULL;
    packet *pPkt_next = NULL;
    LIST_HEAD(packet_list);

    do { 

    printf("what do you want:1.insert a packet 2.del a packet 3.traverse list 4.exit\n");
 
    scanf("%d",&choice);
    printf("choice:%d\n",choice);

    switch (choice) {
    case 1:
        {
            pPkt = (packet *)malloc(sizeof(packet));
            if (NULL == pPkt)
            {
                printf("insert failed, malloc failed\n");
            }
            else
            {
                (void)memset(pPkt, 0, sizeof(packet));
                sprintf(pPkt->name, "packet:%d", packet_index);
                pPkt->index = packet_index++;
                list_add_tail(&pPkt->list_head, &packet_list); 
            }
        };
        break;
    case 2:
        {
            index = 0;
            pPkt = NULL;
            printf("please input del_node_index:");
            scanf("%d",&index);
            printf("try to delete node's index=%d\n",index);
            list_for_each_entry_safe(pPkt, pPkt_next, &packet_list, list_head)
            {
                if (pPkt->index == index)
                {
                    list_del(&pPkt->list_head);
                    free(pPkt);
                    printf("delete ok\n");
                    break;
                }
            }
        };
        break;
    case 3:
        {
            index = 0;
            pPkt = NULL;
            printf("traverse list:\n");
            list_for_each_entry(pPkt, &packet_list, list_head)
            {
                printf("packt.name=%s packet.index=%d\n",pPkt->name,pPkt->index);
            }
        };
        break;

    case 4:
        {
            pPkt = NULL;
            printf("delete all node before exit\n");
            list_for_each_entry_safe(pPkt, pPkt_next, &packet_list, list_head)
            {
                printf("packt.name=%s packet.index=%d\n",pPkt->name,pPkt->index);
                list_del(&pPkt->list_head);
                free(pPkt);
            }
            return 0;
        };
        break;
    };

    } while(1);
    return 0;
}

/**********************************************************************/



