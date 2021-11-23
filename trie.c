#include<stdio.h>
#include<string.h>

int main(void)
{
    unsigned int idx = 0xff;
    unsigned char new_str[200];
        
    printf("hello...welcome to using trie...\n");
    printf("please input your choice: 0-add str 1-find str 2-delete str 3-print trie\n");
   
    scanf("%u", &idx);
    printf("your choice is %u ==> ", idx);

    switch(idx) {
    
    case 0:   
        printf("please input add-str:");
        memset(new_str, 0, sizeof(new_str));
        scanf("%s", new_str);
        printf("%s\n",new_str);
        break;

    case 1:
        printf("please input find-str:");
        memset(new_str, 0, sizeof(new_str));
        scanf("%s", new_str);
        printf("%s\n",new_str);
        break;

    case 2:
        printf("please input del-str:");
        memset(new_str, 0, sizeof(new_str));
        scanf("%s", new_str);
        printf("%s\n",new_str);
        break;

    case 3:
        printf("print trie..");
        break;

    default:
        printf("------------------null...\n");
        break;
    }

    return 0;
}



